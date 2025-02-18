#include "LanTcpServer.h"
#include <QDebug>

#include "LanTcpServer.h"
#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

LanTcpServer::LanTcpServer(quint16 port, QObject *parent)
    : QTcpServer(parent), serverPort(port) {}

LanTcpServer::~LanTcpServer() {
    stopListening();
}

bool LanTcpServer::startListening() {
    if (!listen(QHostAddress::Any, serverPort)) {
        qDebug() << "Не удалось запустить сервер на порту:" << serverPort;
        return false;
    }
    qDebug() << "Сервер запущен на порту:" << serverPort;
    return true;
}

void LanTcpServer::stopListening() {
    for (auto socket : clients.keys()) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    close();
    clients.clear();
    qDebug() << "Сервер остановлен";
}

void LanTcpServer::stopAcceptingPlayers() {
    acceptingPlayers = false;
    qDebug() << "Приём новых игроков остановлен";
}

void LanTcpServer::incomingConnection(qintptr socketDescriptor) {
    if (!acceptingPlayers) {
        qDebug() << "Новое подключение отклонено: Лобби заполнено";
        return;
    }

    auto *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qCritical() << "Ошибка установки сокета:" << socket->errorString();
        socket->deleteLater();
        return;
    }

    connect(socket, &QTcpSocket::readyRead, this, &LanTcpServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &LanTcpServer::onClientDisconnected);

    PlayerConnection player = createPlayerFromSocket(socket);
    clients.insert(socket, player);

    emit playerConnected(player);
    qDebug() << "Игрок подключён:" << player.playerName << player.ipAddress.toString();
}

PlayerConnection LanTcpServer::createPlayerFromSocket(QTcpSocket *socket) {
    QHostAddress ip = socket->peerAddress();
    QString playerName = QString("Player_%1").arg(ip.toString().right(5));
    return PlayerConnection(playerName, ip, false);
}

void LanTcpServer::onReadyRead() {
    auto *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    PlayerConnection player = clients.value(socket);
    emit messageReceived(player, data);
    qDebug() << "Сообщение от" << player.playerName << ":" << QString::fromUtf8(data);
}

void LanTcpServer::onClientDisconnected() {
    auto *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    PlayerConnection player = clients.value(socket);
    clients.remove(socket);
    emit playerDisconnected(player);

    qDebug() << "Игрок отключился:" << player.playerName;
    socket->deleteLater();
}

void LanTcpServer::disconnectPlayer(const PlayerConnection &player) {
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it.value().playerName == player.playerName) {
            it.key()->disconnectFromHost();
            return;
        }
    }
}

void LanTcpServer::sendMessageToAll(const QByteArray &message) {
    for (auto socket : clients.keys()) {
        socket->write(message);
    }
}

void LanTcpServer::sendMessageToPlayer(const PlayerConnection &player, const QByteArray &message) {
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it.value().playerName == player.playerName) {
            it.key()->write(message);
            break;
        }
    }
}
