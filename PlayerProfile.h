#ifndef PLAYERPROFILE_H
#define PLAYERPROFILE_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostInfo>

struct PlayerProfile: public INetworkSerializable{
    QString playerName;

    PlayerProfile (const QString &name) : playerName(name) {}

    QByteArray serialize() const override{
        QByteArray data;

        QDataStream out(&data, QIODevice::WriteOnly);
        out << playerName;

        return data;
    }

    void deserialize(const QByteArray &data) override{
        QDataStream in(data);
        in >> playerName;
    }
};

#endif // PLAYERPROFILE_H
