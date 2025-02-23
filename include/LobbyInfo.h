#ifndef LOBBYINFO_H
#define LOBBYINFO_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostAddress>

/**
 * @brief Represents information about a game lobby, including player capacity and network details.
 */
struct LobbyInfo : public INetworkSerializable {
    QString lobbyName;  ///< The name of the lobby.
    int maxPlayers;     ///< Maximum number of players allowed.
    int currentPlayers; ///< Current number of players in the lobby.
    quint16 tcpPort;    ///< TCP port used for the lobby connection.

    LobbyInfo() = default;

    /**
     * @brief Constructs a LobbyInfo object.
     * @param name The name of the lobby.
     * @param max The maximum number of players.
     * @param current The current number of players.
     * @param port The TCP port of the lobby.
     */
    LobbyInfo(const QString &name, int max, int current, quint16 port)
        : lobbyName(name), maxPlayers(max), currentPlayers(current), tcpPort(port) {}

    /**
     * @brief Serializes the lobby information into a QByteArray for network transmission.
     * @return Serialized data.
     */
    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << lobbyName << maxPlayers << currentPlayers << tcpPort;
        return data;
    }

    /**
     * @brief Deserializes a QByteArray into a LobbyInfo object.
     * @param data The serialized lobby information data.
     */
    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        in >> lobbyName >> maxPlayers >> currentPlayers >> tcpPort;
    }
};

#endif // LOBBYINFO_H
