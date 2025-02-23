#include "LobbyClient.h"
#include <QDebug>

/**
 * @brief Constructs a LobbyClient instance.
 * @param parent The parent QObject.
 */
LobbyClient::LobbyClient(QObject *parent) : QObject(parent) {}

/**
 * @brief Creates and starts hosting a local TCP server for players to join.
 */
void LobbyClient::onHostOwnLocalTcpServer() {
    serverLobby = std::make_unique<ServerLobby>(LOBBY_NAME, MAX_PLAYERS, SERVER_PORT, BROADCAST_PORT, this);

    onConnectToFirstFindedServer();
}

/**
 * @brief Searches for an available lobby and connects to the first one found.
 */
void LobbyClient::onConnectToFirstFindedServer() {
    initClient();
    broadcastListener = std::make_unique<UdpBroadcastListener>(BROADCAST_PORT, this);
    broadcastListener->startListening();
    connect(broadcastListener.get(), &UdpBroadcastListener::lobbyFound, this, &LobbyClient::onLobbyFinded);
}

/**
 * @brief Stops any active game sessions and cleans up network resources.
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
 * @brief Initializes the client for connecting to a lobby.
 */
void LobbyClient::initClient() {
    if (client) return; // Already initialized
    client = std::make_unique<LanTcpClient>(this);

    connect(client.get(), &LanTcpClient::connected, this, [this] {
        if (broadcastListener) broadcastListener->stopListening();
        qDebug() << "Connected to the lobby!";
    });

    connect(client.get(), &LanTcpClient::disconnected, this, [] {
        qDebug() << "Disconnected from the lobby!";
    });

    connect(client.get(), &LanTcpClient::messageReceived, this, [this](const QByteArray &msg) {
        QString message = QString::fromUtf8(msg);
        if (message.trimmed() == "/start") {
            emit invokeGameActionMenu();
        }

        if (message.trimmed() == "/draw") {
            emit invokeResults("No one won this game");
        }

        if (message.trimmed() == "/win") {
            emit invokeResults("Congratulations, you won this game");
        }

        if (message.trimmed() == "/lose") {
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

void LobbyClient::onPlayerMadeChoice(int choice) {
    QString message = "/choice " + QString::number(choice);
    client->sendMessage(message.toUtf8());
}
