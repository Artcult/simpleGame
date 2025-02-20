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
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "Podkluchenie k serveru:" << info.ipAddress.toString() << "Порт:" << info.tcpPort;
        socket->connectToHost(info.ipAddress, info.tcpPort);
    }
    else{
    qDebug() << "Uje Podkluchen!";
    }
   // return;
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
        qDebug() << "Err net podklucheniya k serveru";
    }
}

void LanTcpClient::onConnected() {
    qDebug() << "Podkluchenie ustanovleno";
    emit connected();
}

void LanTcpClient::onDisconnected() {
    qDebug() << "Podkluchenie poterano";
    emit disconnected();
}

void LanTcpClient::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "Polucheno soobcheniye:" << QString::fromUtf8(data);
    emit messageReceived(data);
}
