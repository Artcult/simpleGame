#ifndef LOBBYINFO_H
#define LOBBYINFO_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostAddress>

struct LobbyInfo : public INetworkSerializable {
    QString lobbyName;
    QHostAddress ipAddress;
    int maxPlayers;
    int currentPlayers;
    quint16 tcpPort;

    LobbyInfo() = default;
    LobbyInfo(const QString &name,const QHostAddress &ip, int max, int current, quint16 port)
        : lobbyName(name), ipAddress(ip), maxPlayers(max), currentPlayers(current), tcpPort(port) {}

    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << lobbyName << ipAddress.QHostAddress::toString() << maxPlayers << currentPlayers << tcpPort;
        return data;
    }

    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        QString ipString;
        in >> lobbyName >> ipString >> maxPlayers >> currentPlayers >> tcpPort;
        ipAddress.setAddress(ipString);
    }
};

#endif // LOBBYINFO_H
