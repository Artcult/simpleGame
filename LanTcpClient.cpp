#include "LanTcpClient.h"
#include <QDebug>

LanTcpClient::LanTcpClient(QObject *parent) : QObject(parent), socket(new QTcpSocket(this)) {
    connect(socket, &QTcpSocket::connected, this, &LanTcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &LanTcpClient::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &LanTcpClient::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        emit connectionError(socket->errorString());
    });
}

LanTcpClient::~LanTcpClient() {
    disconnectFromServer();
}

void LanTcpClient::connectToServer(const LobbyInfo &info) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Уже подключен!";
        return;
    }

    qDebug() << "Подключение к серверу:" << info.ipAddress.toString() << "Порт:" << info.tcpPort;
    socket->connectToHost(info.ipAddress, info.tcpPort);
}

void LanTcpClient::disconnectFromServer() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}

void LanTcpClient::sendMessage(const QByteArray &message) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(message);
    } else {
        qDebug() << "Ошибка: нет подключения к серверу!";
    }
}

void LanTcpClient::onConnected() {
    qDebug() << "✅ Подключение к серверу установлено!";
    emit connected();
}

void LanTcpClient::onDisconnected() {
    qDebug() << "❌ Подключение к серверу потеряно.";
    emit disconnected();
}

void LanTcpClient::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "📩 Получено сообщение:" << QString::fromUtf8(data);
    emit messageReceived(data);
}
