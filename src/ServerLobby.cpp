#include "ServerLobby.h"
#include <QDebug>
#include <QNetworkInterface>

/**
 * @brief Constructs a ServerLobby instance, starts the server, and begins broadcasting lobby information.
 * @param lobbyName The name of the lobby.
 * @param maxPlayers The maximum number of players allowed.
 * @param serverPort The TCP port for player connections.
 * @param broadcastPort The UDP port for lobby broadcasting.
 * @param parent The parent QObject.
 */
ServerLobby::ServerLobby(QString lobbyName, int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent)
    : QObject(parent), maxPlayers(maxPlayers), tcpPort(serverPort), udpPort(broadcastPort) {

    // Initialize lobby information
    lobbyInfo = LobbyInfo(lobbyName, maxPlayers, 0, tcpPort);

    // Attempt to start the server, otherwise throw an error
    if (!startServer()) {
        throw std::runtime_error("Server not started");
    }
}

/**
 * @brief Starts the TCP server to accept player connections.
 * @return True if the server starts successfully, otherwise false.
 */
bool ServerLobby::startServer() {
    if (server) return false; // Check if the server is already running

    // Create a TCP server
    server = std::make_unique<LanTcpServer>(tcpPort, this);

    // Connect server signals to their respective handlers
    connect(server.get(), &LanTcpServer::playerConnected, this, &ServerLobby::onPlayerConnected);
    connect(server.get(), &LanTcpServer::playerDisconnected, this, &ServerLobby::onPlayerDisconnected);
    connect(server.get(), &LanTcpServer::messageReceived, this, &ServerLobby::onMessageRecived);

    // Start the server
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
        return startServer(); // If the server is not running, try to restart it
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
 * @param player The connected player.
 */
void ServerLobby::onPlayerConnected(const PlayerConnection &player) {
    if (!server) return;

    // Check if there is space in the lobby
    if (!isRoomAvailable()) {
        server->disconnectPlayer(player);
        return;
    }

    // Check if the player is already connected
    auto it = std::find_if(players.begin(), players.end(), [&](const PlayerConnection &p) {
        return p.playerName == player.playerName && p.ipAddress == player.ipAddress;
    });

    if (it != players.end()) {
        server->disconnectPlayer(player);
        return;
    }

    players.append(player); // Add the player to the list
    refreshLobbyInfo();
}

/**
 * @brief Handles player disconnection by removing them from the list.
 * @param player The player who disconnected.
 */
void ServerLobby::onPlayerDisconnected(const PlayerConnection &player) {
    auto it = std::remove_if(players.begin(), players.end(), [&](const PlayerConnection &p) {
        return p.ipAddress == player.ipAddress;
    });
    players.erase(it, players.end());

    refreshLobbyInfo();
}

/**
 * @brief Processes messages received from players.
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

/**
 * @brief Gets the player ID based on their IP address.
 * @param player The player.
 * @return The player's ID or -1 if not found.
 */
int ServerLobby::getPlayerId(const PlayerConnection &player) {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i].ipAddress == player.ipAddress) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Stores the player's choice (rock, paper, or scissors).
 * @param playerId The player's ID.
 * @param choice The player's choice (1 - rock, 2 - paper, 3 - scissors).
 */
void ServerLobby::playerMove(int playerId, int choice) {
    playerChoices[playerId] = choice;
}

/**
 * @brief Checks if there is room available in the lobby.
 * @return True if space is available, otherwise false.
 */
bool ServerLobby::isRoomAvailable() const {
    return players.size() < maxPlayers;
}

/**
 * @brief Updates the lobby information and starts the game if the lobby is full.
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

/**
 * @brief Determines the winners of the game.
 */
void ServerLobby::calculateWinners() {
    QMap<int, int> choiceCount; // {1 (rock) -> count, 2 (paper) -> count, 3 (scissors) -> count}
    for (const auto &choice : playerChoices) {
        choiceCount[choice]++;
    }

    QList<int> winners;
    if (choiceCount.size() == 3 || choiceCount.size() == 1) {
        // Draw
    } else {
        int winningChoice = (choiceCount.contains(1) && choiceCount.contains(3)) ? 1 :
                                (choiceCount.contains(2) && choiceCount.contains(1)) ? 2 : 3;

        for (auto it = playerChoices.begin(); it != playerChoices.end(); ++it) {
            if (it.value() == winningChoice) {
                winners.append(it.key());
            }
        }
    }

    sendWinnersAndLosers(winners);
}

/**
 * @brief Sends game results (win/loss/draw) to players.
 */
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


