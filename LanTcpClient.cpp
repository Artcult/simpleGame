#include "LanTcpClient.h"
#include <QDebug>

/**
 * @brief Constructs a LanTcpClient instance and initializes the TCP socket.
 *
 * Connects internal socket signals to the appropriate handling methods.
 */
LanTcpClient::LanTcpClient(QObject *parent) : QObject(parent), socket(new QTcpSocket(this)) {
    // Handle successful connection
    connect(socket, &QTcpSocket::connected, this, &LanTcpClient::onConnected);

    // Handle disconnection
    connect(socket, &QTcpSocket::disconnected, this, &LanTcpClient::onDisconnected);

    // Handle incoming messages
    connect(socket, &QTcpSocket::readyRead, this, &LanTcpClient::onReadyRead);

    // Handle connection errors
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        emit connectionError(socket->errorString());
    });
}

/**
 * @brief Destructor.
 *
 * Ensures the client disconnects from the server before destruction.
 */
LanTcpClient::~LanTcpClient() {
    disconnectFromServer();
}

/**
 * @brief Connects to a game server.
 *
 * Attempts to connect only if the client is currently disconnected.
 *
 * @param hostAdress The IP address of the server.
 * @param info The lobby information containing the TCP port.
 */
void LanTcpClient::connectToServer(const QHostAddress &hostAdress, const LobbyInfo &info) {
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        socket->connectToHost(hostAdress, info.tcpPort);
    }
}

/**
 * @brief Disconnects from the server.
 *
 * If the client is connected, it sends a disconnect request and closes the socket.
 */
void LanTcpClient::disconnectFromServer() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}

/**
 * @brief Sends a message to the server.
 *
 * The message is only sent if the client is connected.
 *
 * @param message The data to be sent.
 */
void LanTcpClient::sendMessage(const QByteArray &message) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(message);
    }
}

/**
 * @brief Handles a successful connection event.
 *
 * Emits the `connected` signal.
 */
void LanTcpClient::onConnected() {
    emit connected();
}

/**
 * @brief Handles a disconnection event.
 *
 * Emits the `disconnected` signal.
 */
void LanTcpClient::onDisconnected() {
    emit disconnected();
}

/**
 * @brief Reads incoming messages from the server.
 *
 * Emits the `messageReceived` signal with the received data.
 */
void LanTcpClient::onReadyRead() {
    QByteArray data = socket->readAll();
    emit messageReceived(data);
}
