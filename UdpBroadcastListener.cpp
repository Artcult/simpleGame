#include "UdpBroadcastListener.h"
#include <QDataStream>
#include <QNetworkDatagram>
#include <QDebug>

UdpBroadcastListener::UdpBroadcastListener(quint16 listenPort, QObject *parent)
    : QObject(parent), port(listenPort) {}

void UdpBroadcastListener::startListening() {
    if (udpSocket.bind(QHostAddress::Any, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        connect(&udpSocket, &QUdpSocket::readyRead, this, &UdpBroadcastListener::processPendingDatagrams);
    }
}

void UdpBroadcastListener::stopListening() {
    udpSocket.close();
}

void UdpBroadcastListener::processPendingDatagrams() {
    while (udpSocket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket.receiveDatagram();

        QHostAddress senderIp = datagram.senderAddress();

        QByteArray data = datagram.data();

        LobbyInfo info;
        info.deserialize(data);

        emit lobbyFound(senderIp,info);
    }
}
