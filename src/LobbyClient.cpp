#include "LobbyClient.h"
#include <QDebug>

/**
 * @brief Constructs a LobbyClient instance.
 * @param parent The parent QObject.
 */
LobbyClient::LobbyClient(QObject *parent) : QObject(parent) {}

/**
 * @brief Creates and starts hosting a local TCP server for players to join.
 *        Once hosted, the client automatically connects to the newly created server.
 */
void LobbyClient::onHostOwnLocalTcpServer() {
    serverLobby = std::make_unique<ServerLobby>(LOBBY_NAME, MAX_PLAYERS, SERVER_PORT, BROADCAST_PORT, this);
    onConnectToFirstFindedServer();  // Auto-connect to the hosted server
}

/**
 * @brief Searches for an available lobby and connects to the first one found.
 *        Starts a UDP broadcast listener to discover available game lobbies.
 */
void LobbyClient::onConnectToFirstFindedServer() {
    initClient();
    broadcastListener = std::make_unique<UdpBroadcastListener>(BROADCAST_PORT, this);
    broadcastListener->startListening();
    connect(broadcastListener.get(), &UdpBroadcastListener::lobbyFound, this, &LobbyClient::onLobbyFinded);
}

/**
 * @brief Closes any active game sessions and cleans up networking resources.
 */
void LobbyClient::onCloseGame() {
    if (serverLobby) {
        serverLobby->stopBroadcast();
        serverLobby->stopServer();
        serverLobby.reset();
    }
    if (broadcastListener) {
        broadcastListener->stopListening();
        broadcastListener.reset();
    }
}

/**
 * @brief Initializes the TCP client for connecting to a lobby.
 *        Ensures only one client instance is created.
 */
void LobbyClient::initClient() {
    if (client) return; // Prevent reinitialization
    client = std::make_unique<LanTcpClient>(this);

    connect(client.get(), &LanTcpClient::connected, this, [this] {
        if (broadcastListener) broadcastListener->stopListening();
        qDebug() << "Connected to the lobby!";
    });

    connect(client.get(), &LanTcpClient::disconnected, this, [] {
        qDebug() << "Disconnected from the lobby!";
    });

    // Handles incoming messages from the server
    connect(client.get(), &LanTcpClient::messageReceived, this, [this](const QByteArray &msg) {
        QString message = QString::fromUtf8(msg).trimmed();

        if (message == "/start") {
            emit invokeGameActionMenu();
        } else if (message == "/draw") {
            emit invokeResults("No one won this game");
        } else if (message == "/win") {
            emit invokeResults("Congratulations, you won this game");
        } else if (message == "/lose") {
            emit invokeResults("Sorry, you lost this game");
        }
    });

    connect(client.get(), &LanTcpClient::connectionError, this, [](const QString &error) {
        qDebug() << "Connection error:" << error;
    });
}

/**
 * @brief Connects to a found lobby using its address and information.
 * @param hostAdress The IP address of the found lobby.
 * @param info The lobby's information.
 */
void LobbyClient::onLobbyFinded(const QHostAddress &hostAdress, const LobbyInfo &info) {
    client->connectToServer(hostAdress, info);
}

/**
 * @brief Sends the player's choice to the server.
 * @param choice The player's choice: 1 - Rock, 2 - Paper, 3 - Scissors.
 */
void LobbyClient::onPlayerMadeChoice(int choice) {
    QString message = "/choice " + QString::number(choice);
    client->sendMessage(message.toUtf8());
}

