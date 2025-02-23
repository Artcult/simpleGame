#include "LanTcpServer.h"
#include <QDebug>
#include <QDataStream>
#include <QHostAddress>

/**
 * @brief Constructs the LAN TCP server.
 *
 * @param port The port on which the server will listen.
 * @param parent The parent QObject.
 */
LanTcpServer::LanTcpServer(quint16 port, QObject *parent)
    : QTcpServer(parent), serverPort(port) {}

/**
 * @brief Destructor for the LAN TCP server.
 *
 * Stops the server and disconnects all clients.
 */
LanTcpServer::~LanTcpServer() {
    stopListening();
}

/**
 * @brief Starts listening for player connections.
 *
 * @return true if the server started successfully, false otherwise.
 */
bool LanTcpServer::startListening() {
    setAcceptingPlayers(true);
    return listen(QHostAddress::Any, serverPort);
}

/**
 * @brief Stops the server and disconnects all clients.
 */
void LanTcpServer::stopListening() {
    // Get a list of all connected player sockets
    const QList<QTcpSocket *> sockets = players.keys();

    // Disconnect each client and schedule them for deletion
    for (QTcpSocket *socket : sockets) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }

    // Close the server and clear the player list
    close();
    players.clear();
}

/**
 * @brief Enables or disables accepting new player connections.
 * @param allowNewPlayers If true, allows new players to connect; if false, stops accepting new connections.
 */
void LanTcpServer::setAcceptingPlayers(bool allowNewPlayers) {
    acceptingPlayers = allowNewPlayers;
}

/**
 * @brief Handles an incoming player connection.
 *
 * @param socketDescriptor The descriptor of the new connection.
 */
void LanTcpServer::incomingConnection(qintptr socketDescriptor) {
    if (!acceptingPlayers) return;

    auto *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        socket->deleteLater();
        return;
    }

    connect(socket, &QTcpSocket::readyRead, this, &LanTcpServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &LanTcpServer::onClientDisconnected);

    PlayerConnection player = createPlayerFromSocket(socket);
    players.insert(socket, player);

    emit playerConnected(player);
}

/**
 * @brief Creates a PlayerConnection object from a socket.
 *
 * @param socket The socket associated with the player.
 * @return The generated PlayerConnection object.
 */
PlayerConnection LanTcpServer::createPlayerFromSocket(QTcpSocket *socket) {
    QHostAddress ip = socket->peerAddress();
    QString playerName = QString("Player_%1").arg(ip.toString().right(5));
    return PlayerConnection(playerName, ip, false);
}

/**
 * @brief Reads incoming messages from clients.
 */
void LanTcpServer::onReadyRead() {
    auto *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    PlayerConnection player = players.value(socket);
    emit messageReceived(player, data);
}

/**
 * @brief Handles player disconnections.
 */
void LanTcpServer::onClientDisconnected() {
    auto *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    PlayerConnection player = players.value(socket);
    players.remove(socket);
    emit playerDisconnected(player);

    socket->deleteLater();
}

/**
 * @brief Disconnects a specific player from the server.
 *
 * @param player The player to be disconnected.
 */
void LanTcpServer::disconnectPlayer(const PlayerConnection &player) {
    for (auto it = players.begin(); it != players.end(); ++it) {
        if (it.value().playerName == player.playerName) {
            it.key()->disconnectFromHost();
            return;
        }
    }
}

/**
 * @brief Sends a message to all connected players.
 *
 * @param message The message to be sent.
 */
void LanTcpServer::sendMessageToAll(const QByteArray &message) {
    for (auto it = players.constBegin(); it != players.constEnd(); ++it) {
        it.key()->write(message);
    }
}

/**
 * @brief Sends a message to a specific player.
 *
 * @param player The recipient player.
 * @param message The message to send.
 */
void LanTcpServer::sendMessageToPlayer(const PlayerConnection &player, const QByteArray &message) {
    for (auto it = players.constBegin(); it != players.constEnd(); ++it) {
        if (it.value().playerName == player.playerName) {
            it.key()->write(message);
            break;
        }
    }
}
