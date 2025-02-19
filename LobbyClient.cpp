#include "LobbyClient.h"
#include <QDebug>

LobbyClient::LobbyClient(QObject *parent) : QObject(parent) {}

void LobbyClient::onHostOwnLocalTcpServer() {
    qDebug() << "Создание собственного лобби";
    serverLobby = std::make_unique<ServerLobby>(LOBBY_NAME, MAX_PLAYERS, SERVER_PORT, BROADCAST_PORT, this);
}

void LobbyClient::onConnectToFirstFindedServer() {
    qDebug() << "Поиск и подключение к первому найденному лобби";
    broadcastListener = std::make_unique<UdpBroadcastListener>(BROADCAST_PORT, this);
    connect(broadcastListener.get(), &UdpBroadcastListener::lobbyFound, this, [this](const LobbyInfo &info){
        qDebug() << "Найдено лобби:" << info.lobbyName << "- Подключение...";
        connectToLobby(info);
    });
}

void LobbyClient::onCloseGame() {
    qDebug() << "Закрытие игры и освобождение ресурсов";
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
            qDebug() << "🟢 Подключено к лобби!";
        });

        connect(client.get(), &LanTcpClient::disconnected, this, []() {
            qDebug() << "🔴 Отключено от лобби!";
        });

        connect(client.get(), &LanTcpClient::messageReceived, this, [](const QByteArray &msg) {
            qDebug() << "📨 Сообщение от сервера:" << QString::fromUtf8(msg);
        });

        connect(client.get(), &LanTcpClient::connectionError, this, [](const QString &error) {
            qDebug() << "❌ Ошибка подключения:" << error;
        });
    }

    client->connectToServer(info);
}
