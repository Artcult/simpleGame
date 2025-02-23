#include "UdpBroadcastListener.h"
#include <QDataStream>
#include <QNetworkDatagram>
#include <QDebug>

/**
 * @brief Constructs a UdpBroadcastListener.
 *
 * @param listenPort The port to listen on for incoming UDP messages.
 * @param parent The parent QObject (default is nullptr).
 */
UdpBroadcastListener::UdpBroadcastListener(quint16 listenPort, QObject *parent)
    : QObject(parent), port(listenPort) {}

/**
 * @brief Starts listening for UDP broadcast messages.
 *
 * Binds the UDP socket to the specified port with address sharing enabled.
 * Connects the socket’s `readyRead` signal to handle incoming data.
 */
void UdpBroadcastListener::startListening() {
    if (udpSocket.bind(QHostAddress::Any, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        connect(&udpSocket, &QUdpSocket::readyRead, this, &UdpBroadcastListener::onProcessPendingDatagrams);
    }
}

/**
 * @brief Stops listening for UDP messages.
 *
 * Closes the socket, preventing further message reception.
 */
void UdpBroadcastListener::stopListening() {
    udpSocket.close();
}

/**
 * @brief Processes incoming UDP datagrams.
 *
 * Reads all available datagrams from the socket. For each datagram:
 * - Extracts the sender's IP address.
 * - Deserializes the data into a `LobbyInfo` object.
 * - Emits the `lobbyFound` signal with the extracted information.
 */
void UdpBroadcastListener::onProcessPendingDatagrams() {
    while (udpSocket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket.receiveDatagram();

        QHostAddress senderIp = datagram.senderAddress();
        QByteArray data = datagram.data();

        LobbyInfo info;
        info.deserialize(data);

        emit lobbyFound(senderIp, info);
    }
}

