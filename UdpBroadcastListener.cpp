#include "UdpBroadcastListener.h"
#include <QDataStream>
#include <QNetworkDatagram>
#include <QDebug>

UdpBroadcastListener::UdpBroadcastListener(quint16 listenPort, QObject *parent)
    : QObject(parent), port(listenPort) {}

void UdpBroadcastListener::startListening() {
    if (udpSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)) {
        connect(&udpSocket, &QUdpSocket::readyRead, this, &UdpBroadcastListener::processPendingDatagrams);
        qDebug() << "Начат прием UDP-броадкастов на порту" << port;
    } else {
        qCritical() << "Ошибка привязки порта" << port;
    }
}

void UdpBroadcastListener::stopListening() {
    udpSocket.close();
    qDebug() << "Прием UDP-броадкастов остановлен";
}

void UdpBroadcastListener::processPendingDatagrams() {
    while (udpSocket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket.receiveDatagram();
        QByteArray data = datagram.data();
        data.resize(datagram.data().size()); // Ресайзим дату перед десериализацией

        LobbyInfo info;
        info.deserialize(data);
        emit lobbyFound(info);
        qDebug() << "Получен UDP-броадкаст: Лобби" << info.lobbyName;
    }
}
