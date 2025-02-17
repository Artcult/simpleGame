#ifndef PLAYERCONNECTION_H
#define PLAYERCONNECTION_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostAddress>

struct PlayerConnection : public INetworkSerializable {
    QString playerName;
    QHostAddress ipAddress;
    bool isHost = false;

    PlayerConnection() = default;
    PlayerConnection(const QString &name, const QHostAddress &ip, bool host)
        : playerName(name), ipAddress(ip), isHost(host) {}

    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << playerName << ipAddress.toString() << isHost;
        return data;
    }

    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        QString ipString;
        in >> playerName >> ipString >> isHost;
        ipAddress = QHostAddress(ipString);
    }
};

#endif // PLAYERCONNECTION_H
