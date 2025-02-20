#include "LobbyClient.h"
#include <QDebug>

LobbyClient::LobbyClient(QObject *parent) : QObject(parent) {}

void LobbyClient::onHostOwnLocalTcpServer() {
    qDebug() << "sozdanie lobby";
    serverLobby = std::make_unique<ServerLobby>(LOBBY_NAME, MAX_PLAYERS, SERVER_PORT, BROADCAST_PORT, this);
}

void LobbyClient::onConnectToFirstFindedServer() {
    qDebug() << "poyisk i podkluchenie k lobby";
    broadcastListener = std::make_unique<UdpBroadcastListener>(BROADCAST_PORT, this);
    connect(broadcastListener.get(), &UdpBroadcastListener::lobbyFound, this, [this](const LobbyInfo &info){
        qDebug() << "Найдено лобби:" << info.lobbyName << "- Подключение...";
        connectToLobby(info);
    });
}

void LobbyClient::onCloseGame() {
    qDebug() << "zacritiye";
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

void LobbyClient::connectToLobby(const LobbyInfo &info) {
    if (!client) {
        client = std::make_unique<LanTcpClient>(this);

        connect(client.get(), &LanTcpClient::connected, this, [this]() {
            if (broadcastListener) {
                broadcastListener->stopListening();
            }
            qDebug() << "podklucheno k lobby!";
        });

        connect(client.get(), &LanTcpClient::disconnected, this, []() {
            qDebug() << "otklucheno ot lobby!";
        });

        connect(client.get(), &LanTcpClient::messageReceived, this, [](const QByteArray &msg) {
            qDebug() << "soobchenie ot servera:" << QString::fromUtf8(msg);
        });

        connect(client.get(), &LanTcpClient::connectionError, this, [](const QString &error) {
            qDebug() << "err podcluchenia:" << error;
        });
    }

    client->connectToServer(info);
}
