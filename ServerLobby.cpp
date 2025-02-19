#include "ServerLobby.h"
#include <QDebug>

ServerLobby::ServerLobby(QString lobbyName,int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent)
    : QObject(parent), maxPlayers(maxPlayers), tcpPort(serverPort), udpPort(broadcastPort) {

    lobbyInfo = LobbyInfo(lobbyName, QHostAddress::LocalHost, maxPlayers,0, tcpPort);

    if (!startServer()) {
        qDebug() << "Ошибка при запуске лобби!";
        throw std::runtime_error("Не удалось запустить сервер");
    }

    startBroadcast();

}

bool ServerLobby::startServer() {
    server = std::make_unique<LanTcpServer>(tcpPort, this);

    connect(server.get(), &LanTcpServer::playerConnected, this, &ServerLobby::onPlayerConnected);
    connect(server.get(), &LanTcpServer::playerDisconnected, this, &ServerLobby::onPlayerDisconnected);
    connect(server.get(), &LanTcpServer::messageReceived, this, [](const PlayerConnection &player, const QByteArray &msg){
        qDebug() << "Сообщение от" << player.playerName << ":" << msg;
    });

    if (server->startListening()) {
        qDebug() << "Лобби сервер запущен на порту:" << tcpPort;

        // Подключаем хоста к серверу
        connectAsHost();

        // Обновляем информацию о лобби
        refreshLobbyInfo();

        return true;
    } else {
        qDebug() << "Ошибка запуска лобби сервера!";
        return false;
    }
}

void ServerLobby::stopServer() {
    if (server) {
        server->stopAcceptingPlayers();
        server->stopListening();
    }
    players.clear();
    qDebug() << "TCP сервер остановлен";
}

void ServerLobby::startBroadcast() {
    broadcaster = std::make_unique<UdpBroadcaster>(udpPort, this);
    connect(broadcaster.get(), &UdpBroadcaster::broadcastTick, this, &ServerLobby::updateLobbyInfo);
    broadcaster->startBroadcast(lobbyInfo);
}

void ServerLobby::stopBroadcast() {
    broadcaster->stopBroadcast();
    qDebug() << "Броадкаст остановлен";
}

bool ServerLobby::connectAsHost() {
    // Создаем подключение хоста
    PlayerConnection host("Host", QHostAddress::LocalHost, true);

    if (!isRoomAvailable()) {
        qWarning() << "Лобби заполнено, хост не может подключиться";
        return false;
    }

    players.append(host);
    emit playerConnected(host); // Сигнал для обновления UI или других модулей

    qDebug() << "Хост подключен как:" << host.playerName;
    return true;
}

void ServerLobby::onPlayerConnected(const PlayerConnection &player) {
    // Проверяем на дублирование игроков
    for (const auto &existingPlayer : players) {
        if (existingPlayer.playerName == player.playerName) {
            qWarning() << "Игрок с именем" << player.playerName << "уже в лобби!";
            server->disconnectPlayer(player);
            return;
        }
    }

    if (isRoomAvailable()) {
        players.append(player);
        qDebug() << player.playerName << "подключился.";
    } else {
        qWarning() << "Лобби заполнено, игрок отклонён:" << player.playerName;
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
        qDebug() << "Лобби заполнено, стартуем игру!";
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


