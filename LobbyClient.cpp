#include "LobbyClient.h"
#include <QDebug>

LobbyClient::LobbyClient(QObject *parent) : QObject(parent) {}

void LobbyClient::onHostOwnLocalTcpServer() {
    qDebug() << "Создание собственного лобби";
    serverLobby = std::make_unique<ServerLobby>(LOBBY_NAME, MAX_PLAYERS, SERVER_PORT, BROADCAST_PORT, this);
    serverLobby->startServer();
    serverLobby->startBroadcast();
}

void LobbyClient::onConnectToFirstFindedServer() {
    qDebug() << "Поиск и подключение к первому найденному лобби";
    broadcastListener = std::make_unique<UdpBroadcastListener>(BROADCAST_PORT, this);
    connect(broadcastListener.get(), &UdpBroadcastListener::lobbyFound, this, [](const LobbyInfo &info){
        qDebug() << "Найдено лобби:" << info.lobbyName << "- Подключение...";
    });
    broadcastListener->startListening();
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
