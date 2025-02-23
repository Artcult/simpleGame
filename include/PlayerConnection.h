#ifndef PLAYERCONNECTION_H
#define PLAYERCONNECTION_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostAddress>

/**
 * @brief Represents a player's connection details in the lobby.
 */
struct PlayerConnection : public INetworkSerializable {
    QString playerName; ///< Name of the player.
    QHostAddress ipAddress; ///< IP address of the player.
    bool isHost = false; ///< Indicates if the player is the host.

    /**
     * @brief Default constructor.
     */
    PlayerConnection() = default;

    /**
     * @brief Constructs a PlayerConnection object.
     * @param name Player's name.
     * @param ip Player's IP address.
     * @param host Whether the player is the host.
     */
    PlayerConnection(const QString &name, const QHostAddress &ip, bool host)
        : playerName(name), ipAddress(ip), isHost(host) {}

    /**
     * @brief Serializes the PlayerConnection object into a QByteArray.
     * @return Serialized data.
     */
    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << playerName << ipAddress.toString() << isHost;
        return data;
    }

    /**
     * @brief Deserializes the PlayerConnection object from a QByteArray.
     * @param data Serialized data.
     */
    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        QString ipString;
        in >> playerName >> ipString >> isHost;
        ipAddress = QHostAddress(ipString);
    }
};

#endif // PLAYERCONNECTION_H
