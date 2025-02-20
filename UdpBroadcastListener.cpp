#include "UdpBroadcastListener.h"
#include <QDataStream>
#include <QNetworkDatagram>
#include <QDebug>

UdpBroadcastListener::UdpBroadcastListener(quint16 listenPort, QObject *parent)
    : QObject(parent), port(listenPort) {
    startListening();
}

void UdpBroadcastListener::startListening() {
    if (udpSocket.bind(QHostAddress::Any, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        connect(&udpSocket, &QUdpSocket::readyRead, this, &UdpBroadcastListener::processPendingDatagrams);
        qDebug() << "Listen UDP on port" << port;
    } else {
        qCritical() << "Port async error" << port;
    }
}

void UdpBroadcastListener::stopListening() {
    udpSocket.close();
    qDebug() << "Stop Listen UDP";
}

void UdpBroadcastListener::processPendingDatagrams() {
    while (udpSocket.hasPendingDatagrams()) {
        // Получаем датаграмму
        QNetworkDatagram datagram = udpSocket.receiveDatagram();

        // IP-адрес отправителя
        QHostAddress senderIp = datagram.senderAddress();
        quint16 senderPort = datagram.senderPort();

        // Получаем данные
        QByteArray data = datagram.data();

        // Десериализуем LobbyInfo
        LobbyInfo info;
        info.deserialize(data);

        // Устанавливаем IP-адрес отправителя в LobbyInfo
        info.ipAddress = senderIp;

        // Отправляем сигнал о найденном лобби
        emit lobbyFound(info);

        qDebug() << "Received UDP Lobby:" << info.lobbyName
                 << "from IP:" << info.ipAddress;
    }
}
