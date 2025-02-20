#include "UdpBroadcaster.h"
#include <QDataStream>
#include <QNetworkDatagram>
#include <QDebug>
#include <QNetworkInterface>
#include <QUuid>

UdpBroadcaster::UdpBroadcaster(quint16 broadcastPort, QObject *parent)
    : QObject(parent), port(broadcastPort) {
    updateAddresses();
    connect(&broadcastTimer, &QTimer::timeout, this, &UdpBroadcaster::sendBroadcast);
}

void UdpBroadcaster::startBroadcast(const LobbyInfo &info) {
    currentLobbyInfo = info;
    broadcastTimer.start(1000); // Отправляем каждую секунду
    qDebug() << "Start UDP Broadcast" << port;
}

void UdpBroadcaster::stopBroadcast() {
    broadcastTimer.stop();
    qDebug() << "UDP Broadcast stopped";
}

void UdpBroadcaster::sendBroadcast() {
    QByteArray data = currentLobbyInfo.serialize();
    bool validBroadcastAddresses = true;
    for(const QHostAddress &address : std::as_const(broadcastAddresses)){
        if (udpSocket.writeDatagram(data, address, port) == -1)
          validBroadcastAddresses = false;
    }
    if (!validBroadcastAddresses)
        updateAddresses();
    emit broadcastTick();
}

void UdpBroadcaster::updateAddresses()
{
    broadcastAddresses.clear();
    ipAddresses.clear();
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
        const QList<QNetworkAddressEntry> entries = interface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            QHostAddress broadcastAddress = entry.broadcast();
            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost) {
                broadcastAddresses << broadcastAddress;
                ipAddresses << entry.ip();
            }
        }
    }
}
