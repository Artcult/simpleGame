#include "UdpBroadcaster.h"
#include <QDataStream>
#include <QNetworkDatagram>
#include <QDebug>

UdpBroadcaster::UdpBroadcaster(quint16 broadcastPort, QObject *parent)
    : QObject(parent), port(broadcastPort) {
    connect(&broadcastTimer, &QTimer::timeout, this, &UdpBroadcaster::sendBroadcast);
}

void UdpBroadcaster::startBroadcast(const LobbyInfo &info) {
    currentLobbyInfo = info;
    broadcastTimer.start(1000); // Отправляем каждую секунду
    qDebug() << "Начат UDP-броадкаст на порту" << port;
}

void UdpBroadcaster::stopBroadcast() {
    broadcastTimer.stop();
    qDebug() << "UDP-броадкаст остановлен";
}

void UdpBroadcaster::sendBroadcast() {
    QByteArray data = currentLobbyInfo.serialize();
    udpSocket.writeDatagram(data, QHostAddress::Broadcast, port);
    emit broadcastTick();
}
