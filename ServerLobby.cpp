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
    QString message = QString::fromUtf8(msg).trimmed();

    if (message.startsWith("/choice")) {
        QStringList parts = message.split(" ");
        if (parts.size() == 2) {
            int choice = parts[1].toInt();

            int playerId = getPlayerId(player);
            playerMove(playerId, choice);

            if (playerChoices.size() == players.size()) {
                calculateWinners();
            }
        }
    }
}

int ServerLobby::getPlayerId(const PlayerConnection &player) {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i].ipAddress == player.ipAddress) {
            return i;
        }
    }
    return -1;
}

void ServerLobby::playerMove(int playerId, int choice) {
    playerChoices[playerId] = choice;
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
    QString startMessage = "/start";
    for (const PlayerConnection &player : std::as_const(players)) {
        qDebug() << player.playerName << "@" << player.ipAddress.toString();
    }
    server->sendMessageToAll(startMessage.toUtf8());

}

void ServerLobby::calculateWinners() {
    QMap<int, int> choiceCount; // {1 (камень) -> количество, 2 (бумага) -> количество, 3 (ножницы) -> количество}
    for (const auto &choice : playerChoices) {
        choiceCount[choice]++;
    }

    QList<int> winners;
    if (choiceCount.size() == 3 || choiceCount.size() == 1) {
    } else {
        int winningChoice = 0;
        if (choiceCount.contains(1) && choiceCount.contains(3)) {
            winningChoice = 1; // Камень бьет ножницы
        } else if (choiceCount.contains(2) && choiceCount.contains(1)) {
            winningChoice = 2; // Бумага бьет камень
        } else if (choiceCount.contains(3) && choiceCount.contains(2)) {
            winningChoice = 3; // Ножницы бьют бумагу
        }

        for (auto it = playerChoices.begin(); it != playerChoices.end(); ++it) {
            if (it.value() == winningChoice) {
                winners.append(it.key());
            }
        }
    }

    sendWinnersAndLosers(winners);
}

void ServerLobby::sendWinnersAndLosers(const QList<int> &winners) {
    QString drawMessage = "/draw";
    QString winMessage = "/win";
    QString loseMessage = "/lose";

    if (winners.isEmpty()) {

        for (const auto &player : players) {
            server->sendMessageToPlayer(player,drawMessage.toUtf8());
        }
        return;
    }

    for (const auto &player : players) {
        int playerId = getPlayerId(player);
        if (winners.contains(playerId)) {
            server->sendMessageToPlayer(player, winMessage.toUtf8());
        } else {
            server->sendMessageToPlayer(player, loseMessage.toUtf8());
        }
    }
}
