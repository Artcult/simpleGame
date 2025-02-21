#include "ServerLobby.h"
#include <QDebug>
#include <QNetworkInterface>

ServerLobby::ServerLobby(QString lobbyName,int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent)
    : QObject(parent), maxPlayers(maxPlayers), tcpPort(serverPort), udpPort(broadcastPort) {

    lobbyInfo = LobbyInfo(lobbyName, QHostAddress::Any, maxPlayers,0, tcpPort);

    if (!startServer()) {
        qDebug() << "Err to start lobby!";
        throw std::runtime_error("server not started");
    }

    startBroadcast();

}

bool ServerLobby::startServer() {
    server = std::make_unique<LanTcpServer>(tcpPort, this);

    connect(server.get(), &LanTcpServer::playerConnected, this, &ServerLobby::onPlayerConnected);
    connect(server.get(), &LanTcpServer::playerDisconnected, this, &ServerLobby::onPlayerDisconnected);
    connect(server.get(), &LanTcpServer::messageReceived, this, [](const PlayerConnection &player, const QByteArray &msg){
        qDebug() << "Message from " << player.playerName << ":" << msg;
    });

    if (server->startListening()) {
        qDebug() << "Lobby start on port:" << tcpPort;
        // Подключаем хоста к серверу
        connectAsHost();

        // Обновляем информацию о лобби
        refreshLobbyInfo();

        QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
        for (const QHostAddress &addr : ipList) {
            qDebug() << "Accessing IP:" << addr.toString();
        }

        return true;
    } else {
        qDebug() << "Err Start Lobby!";
        return false;
    }


}

void ServerLobby::stopServer() {
    if (server) {
        server->stopAcceptingPlayers();
        server->stopListening();
    }
    players.clear();
    qDebug() << "TCP server stopped";
}

void ServerLobby::startBroadcast() {
    if (!broadcaster) {
        broadcaster = std::make_unique<UdpBroadcaster>(udpPort, this);
        connect(this, &ServerLobby::lobbyInfoUpdated, broadcaster.get(), &UdpBroadcaster::refreshLobbyInfo);
    }

    broadcaster->startBroadcast(lobbyInfo);
}

void ServerLobby::stopBroadcast() {
    broadcaster->stopBroadcast();
}

bool ServerLobby::connectAsHost() {
    // Создаем подключение хоста
    PlayerConnection host("Host", QHostAddress::Any, true);

    if (!isRoomAvailable()) {
        qWarning() << "Lobby is full, sorry";
        return false;
    }

    players.append(host);
    emit playerConnected(host); // Сигнал для обновления UI или других модулей

    qDebug() << "Host append as:" << host.playerName;
    return true;
}

void ServerLobby::onPlayerConnected(const PlayerConnection &player) {
    // Проверяем на дублирование игроков
    for (const auto &existingPlayer : players) {
        if (existingPlayer.playerName == player.playerName) {
            qWarning() << "Player with name" << player.playerName << "already in Lobby";
            server->disconnectPlayer(player);
            return;
        }
    }

    if (isRoomAvailable()) {
        players.append(player);
        qDebug() << player.playerName << "appended";
    } else {
        qWarning() << "Lobby is full, player leave" << player.playerName;
        server->disconnectPlayer(player);
    }
    refreshLobbyInfo();
}


void ServerLobby::onPlayerDisconnected(const PlayerConnection &player) {
    auto it = std::remove_if(players.begin(), players.end(), [&](const PlayerConnection &p) {
        return p.ipAddress == player.ipAddress;  // Удаляем по IP, т.к. имена могут совпадать
    });
    players.erase(it, players.end());

    qDebug() << player.playerName << "отключился.";
    refreshLobbyInfo();
}

bool ServerLobby::isRoomAvailable() const {
    return players.size() < maxPlayers;
}

void ServerLobby::refreshLobbyInfo() {
    lobbyInfo.currentPlayers = players.size();
    emit lobbyInfoUpdated(lobbyInfo);
}

void ServerLobby::updateLobbyInfo() {
    refreshLobbyInfo();

    if (players.size() >= maxPlayers) {
        qDebug() << "Lobby is full? sart game!";
        server->stopAcceptingPlayers();

        // Проверяем, все ли игроки еще в сети перед стартом
        if (players.size() == maxPlayers) {
            startGame();
        } else {
            qWarning() << "Игроки отключились, отменяем старт игры!";
        }
    }
}

void ServerLobby::startGame() {

    // Отправляем сигнал с текущими игроками
    emit gameStarting(players);

    qDebug() << "Запуск игры. Игроков в лобби:" << players.size();

    qDebug() << "Игра начинается с игроками:";
    for (const auto &player : players) {
        qDebug() << player.playerName << "@" << player.ipAddress.toString();
    }

    // Останавливаем бродкаст и сервер для новых подключений
    stopBroadcast();
    server->stopAcceptingPlayers();
}


