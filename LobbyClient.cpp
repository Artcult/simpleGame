#include "LobbyClient.h"
#include <QDebug>

LobbyClient::LobbyClient(QObject *parent) : QObject(parent) {}

void LobbyClient::onConnectToFirstFindedServer(){
    qDebug() << "Go search free server";
}

void LobbyClient::onHostOwnLocalTcpServer(){
    qDebug() << "Go create own server";
}

void LobbyClient::onCloseGame(){
    qDebug() << "Bruh.. bye, bye!";
}
