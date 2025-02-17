#include "ServerLobby.h"
#include <QDebug>

ServerLobby::ServerLobby(int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent)
    : QObject(parent), maxPlayers(maxPlayers), port(serverPort), broadcastPort(broadcastPort) {
    server = std::make_unique<LanTcpServer>(this);
    broadcaster = std::make_unique<UdpBroadcaster>(this);
}

bool ServerLobby::startLobby() {
    if (!server->start(port)) {
        qCritical() << "Error: Failed to start server on port" << port;
        return false;
    }
    connect(server.get(), &LanTcpServer::playerConnected, this, &ServerLobby::onPlayerConnected);
    connect(server.get(), &LanTcpServer::playerDisconnected, this, &ServerLobby::onPlayerDisconnected);
    qDebug() << "Lobby started on TCP port" << port;
    return true;
}

void ServerLobby::stopLobby() {
    server->stop();
    stopBroadcast();
    players.clear();
    qDebug() << "Lobby stopped.";
}

void ServerLobby::startBroadcast() {
    if (!broadcaster->startBroadcast(broadcastPort)) {
        qCritical() << "Error: Failed to start UDP broadcast on port" << broadcastPort;
        emit broadcastStatusChanged(false);
    } else {
        emit broadcastStatusChanged(true);
        qDebug() << "Broadcast started on UDP port" << broadcastPort;
    }
}

void ServerLobby::stopBroadcast() {
    broadcaster->stopBroadcast();
    emit broadcastStatusChanged(false);
    qDebug() << "Broadcast stopped.";
}

bool ServerLobby::connectAsHost(const QString &hostAddress) {
    if (!isRoomAvailable()) {
        emit hostConnectionStatus(false);
        qWarning() << "Host connection failed: Lobby full.";
        return false;
    }
    if (players.contains(PlayerConnection(hostAddress, true))) {
        emit hostConnectionStatus(false);
        qWarning() << "Host connection failed: Already connected.";
        return false;
    }
    players.append(PlayerConnection(hostAddress, true));
    emit hostConnectionStatus(true);
    emit playerCountChanged(players.size());
    qDebug() << "Host connected from" << hostAddress;
    return true;
}

QString ServerLobby::getLobbyInfo() const {
    return QString("Players: %1/%2").arg(players.size()).arg(maxPlayers);
}

void ServerLobby::onPlayerConnected(const PlayerConnection &player) {
    if (isRoomAvailable() && !players.contains(player)) {
        players.append(player);
        emit playerCountChanged(players.size());
        qDebug() << "Player connected from" << player.getAddress();
    } else {
        qWarning() << "Connection refused: Room full or duplicate player.";
    }
}

void ServerLobby::onPlayerDisconnected(const PlayerConnection &player) {
    players.removeAll(player);
    emit playerCountChanged(players.size());
    qDebug() << "Player disconnected from" << player.getAddress();
}

bool ServerLobby::isRoomAvailable() const {
    return players.size() < maxPlayers;
}

