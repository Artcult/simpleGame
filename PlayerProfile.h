#ifndef PLAYERPROFILE_H
#define PLAYERPROFILE_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostInfo>

/**
 * @brief Represents a player's profile.
 */
struct PlayerProfile : public INetworkSerializable {
    QString playerName; ///< Name of the player.

    /**
     * @brief Constructs a PlayerProfile object.
     * @param name Player's name.
     */
    PlayerProfile(const QString &name) : playerName(name) {}

    /**
     * @brief Serializes the PlayerProfile object into a QByteArray.
     * @return Serialized data.
     */
    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << playerName;
        return data;
    }

    /**
     * @brief Deserializes the PlayerProfile object from a QByteArray.
     * @param data Serialized data.
     */
    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        in >> playerName;
    }
};

#endif // PLAYERPROFILE_H
