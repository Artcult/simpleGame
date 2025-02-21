#ifndef LOBBYINFO_H
#define LOBBYINFO_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostAddress>

struct LobbyInfo : public INetworkSerializable {
    QString lobbyName;
    int maxPlayers;
    int currentPlayers;
    quint16 tcpPort;

    LobbyInfo() = default;
    LobbyInfo(const QString &name, int max, int current, quint16 port)
        : lobbyName(name), maxPlayers(max), currentPlayers(current), tcpPort(port) {}

    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << lobbyName << maxPlayers << currentPlayers << tcpPort;
        return data;
    }

    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        in >> lobbyName >> maxPlayers >> currentPlayers >> tcpPort;
    }
};

#endif // LOBBYINFO_H
