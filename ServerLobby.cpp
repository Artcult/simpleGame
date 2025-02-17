#include "ServerLobby.h"
#include <QDebug>

ServerLobby::ServerLobby(int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent)
    : QObject(parent), maxPlayers(maxPlayers), tcpPort(serverPort), udpPort(broadcastPort) {
    server = std::make_unique<LanTcpServer>(this);
    broadcaster = std::make_unique<UdpBroadcaster>(this);
    lobbyInfo = LobbyInfo("DefaultLobby", tcpPort, maxPlayers);
}

bool ServerLobby::startLobby() {
    if (!server->start(tcpPort)) {
        qCritical() << "Error: Failed to start server on port" << tcpPort;
        return false;
    }
    connect(server.get(), &LanTcpServer::playerConnected, this, &ServerLobby::onPlayerConnected);
    connect(server.get(), &LanTcpServer::playerDisconnected, this, &ServerLobby::onPlayerDisconnected);
    startBroadcast();
    qDebug() << "Lobby started on TCP port" << tcpPort;
    return true;
}

void ServerLobby::stopLobby() {
    server->stop();
    stopBroadcast();
    players.clear();
    qDebug() << "Lobby stopped.";
}

void ServerLobby::onPlayerConnected(const PlayerConnection &player) {
    if (isRoomAvailable() && !players.contains(player)) {
        players.append(player);
        updateLobbyInfo();
        emit playerCountChanged(players.size());
        qDebug() << "Player connected from" << player.getAddress();
    }
}

void ServerLobby::onPlayerDisconnected(const PlayerConnection &player) {
    players.removeAll(player);
    updateLobbyInfo();
    emit playerCountChanged(players.size());
    qDebug() << "Player disconnected from" << player.getAddress();
}

void ServerLobby::updateLobbyInfo() {
    lobbyInfo.currentPlayers = players.size();
    broadcaster->sendLobbyInfo(lobbyInfo);
}

void ServerLobby::startBroadcast() {
    if (!broadcaster->startBroadcast(udpPort)) {
        qCritical() << "Error: Failed to start UDP broadcast on port" << udpPort;
        emit broadcastStatusChanged(false);
    } else {
        emit broadcastStatusChanged(true);
        qDebug() << "Broadcast started on UDP port" << udpPort;
    }
}

