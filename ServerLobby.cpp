#include "ServerLobby.h"
#include <QDebug>
#include <QNetworkInterface>

/**
 * @brief Constructs a ServerLobby instance and starts the server and broadcast.
 * @param lobbyName The name of the lobby.
 * @param maxPlayers The maximum number of players allowed.
 * @param serverPort The TCP port for player connections.
 * @param broadcastPort The UDP port for lobby broadcasting.
 * @param parent The parent QObject.
 */
ServerLobby::ServerLobby(QString lobbyName, int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent)
    : QObject(parent), maxPlayers(maxPlayers), tcpPort(serverPort), udpPort(broadcastPort) {

    lobbyInfo = LobbyInfo(lobbyName, maxPlayers, 0, tcpPort);

    if (!startServer()) {
        throw std::runtime_error("server not started");
    }
}

/**
 * @brief Starts the TCP server to accept player connections.
 * @return True if the server started successfully, otherwise false.
 */
bool ServerLobby::startServer() {
    if (server) return false;

    server = std::make_unique<LanTcpServer>(tcpPort, this);

    connect(server.get(), &LanTcpServer::playerConnected, this, &ServerLobby::onPlayerConnected);
    connect(server.get(), &LanTcpServer::playerDisconnected, this, &ServerLobby::onPlayerDisconnected);
    connect(server.get(), &LanTcpServer::messageReceived, this, &ServerLobby::onMessageRecived);

    if (!server->startListening()) {
        server.reset();
        return false;
    }

    connectAsHost();
    refreshLobbyInfo();
    return true;
}

/**
 * @brief Stops the server and clears the player list.
 */
void ServerLobby::stopServer() {
    server.reset();
    players.clear();
}

/**
 * @brief Pauses lobby search by stopping new player connections and broadcasts.
 */
void ServerLobby::pauseLobbySearch() {
    if (server) {
        server->setAcceptingPlayers(false);
        stopBroadcast();
    }
}

/**
 * @brief Resumes lobby search by allowing new connections and restarting the broadcast.
 * @return True if the operation was successful, otherwise false.
 */
bool ServerLobby::resumeLobbySearch() {
    if (!server) {
        return startServer();
    }
    server->setAcceptingPlayers(true);
    startBroadcast();
    return true;
}

/**
 * @brief Starts broadcasting lobby information over UDP.
 */
void ServerLobby::startBroadcast() {
    if (!broadcaster) {
        broadcaster = std::make_unique<UdpBroadcaster>(udpPort, this);
        connect(this, &ServerLobby::lobbyInfoUpdated, broadcaster.get(), &UdpBroadcaster::onRefreshLobbyInfo);
    }

    broadcaster->startBroadcast(lobbyInfo);
}

/**
 * @brief Stops broadcasting lobby information.
 */
void ServerLobby::stopBroadcast() {
    if (broadcaster) {
        broadcaster->stopBroadcast();
    }

}

/**
 * @brief Assigns the host role to the first player if no host exists.
 * @return True if the host was successfully assigned, otherwise false.
 */
bool ServerLobby::connectAsHost() {
    if (!isRoomAvailable()) {
        qWarning() << "Lobby is full, sorry";
        return false;
    }

    auto it = std::find_if(players.begin(), players.end(), [](const PlayerConnection &p) {
        return p.isHost;
    });

    if (it != players.end()) {
        return false;
    }

    PlayerConnection host("Host", QHostAddress::Any, true);
    players.append(host);
    emit playerConnected(host);

    return true;
}

/**
 * @brief Handles a new player connection.
 * @param player The player that connected.
 */
void ServerLobby::onPlayerConnected(const PlayerConnection &player) {
    if (!server) return;

    if (!isRoomAvailable()) {
        server->disconnectPlayer(player);
        return;
    }

    auto it = std::find_if(players.begin(), players.end(), [&](const PlayerConnection &p) {
        return p.playerName == player.playerName && p.ipAddress == player.ipAddress;
    });

    if (it != players.end()) {
        server->disconnectPlayer(player);
        return;
    }

    players.append(player);

    refreshLobbyInfo();
}

/**
 * @brief Handles player disconnection by removing them from the list.
 * @param player The player that disconnected.
 */
void ServerLobby::onPlayerDisconnected(const PlayerConnection &player) {
    auto it = std::remove_if(players.begin(), players.end(), [&](const PlayerConnection &p) {
        return p.ipAddress == player.ipAddress;  // Remove by IP, as names may be duplicated
    });
    players.erase(it, players.end());

    refreshLobbyInfo();
}

/**
 * @brief Handles messages received from players.
 * @param player The sender of the message.
 * @param msg The message content.
 */
void ServerLobby::onMessageRecived(const PlayerConnection &player, const QByteArray &msg) {
    qDebug() << "Message received from" << player.playerName;
}

/**
 * @brief Checks if there is space available in the lobby.
 * @return True if there is space, otherwise false.
 */
bool ServerLobby::isRoomAvailable() const {
    return players.size() < maxPlayers;
}

/**
 * @brief Updates the lobby information and broadcasts changes.
 */
void ServerLobby::refreshLobbyInfo() {
    lobbyInfo.currentPlayers = players.size();
    emit lobbyInfoUpdated(lobbyInfo);

    if (players.size() >= maxPlayers) {
        pauseLobbySearch();

        if (players.size() == maxPlayers) {
            startGame();
        }
    } else {
        resumeLobbySearch();
    }
}

/**
 * @brief Starts the game when the lobby is full.
 */
void ServerLobby::startGame() {
    emit gameStarting(players);

    for (const PlayerConnection &player : std::as_const(players)) {
        qDebug() << player.playerName << "@" << player.ipAddress.toString();
    }
}


