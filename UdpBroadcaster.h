#ifndef UDPBROADCASTER_H
#define UDPBROADCASTER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QDataStream>
#include "LobbyInfo.h"

/**
 * @brief A class for broadcasting UDP messages within a local network.
 *
 * This class periodically sends lobby information using a UDP socket
 * to all detected broadcast addresses in the network.
 */
class UdpBroadcaster : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a UdpBroadcaster.
     *
     * Initializes the UDP broadcaster with the specified port, updates the
     * list of broadcast addresses, and sets up a timer for periodic transmissions.
     *
     * @param broadcastPort The port used for broadcasting messages.
     * @param parent The parent QObject (default is nullptr).
     */
    explicit UdpBroadcaster(const quint16 broadcastPort, QObject *parent = nullptr);

    /**
     * @brief Starts periodic broadcasting of lobby information.
     *
     * Serializes the provided lobby information and stores it for repeated transmission.
     * If the broadcast timer is not already running, it starts sending messages.
     *
     * @param lobbyInfo The lobby information to be broadcasted.
     */
    void startBroadcast(const LobbyInfo &lobbyInfo);

    /**
     * @brief Stops broadcasting messages.
     *
     * Disables the broadcast timer, halting further transmissions.
     */
    void stopBroadcast();

signals:
    /**
     * @brief Emitted after each successful broadcast message.
     */
    void broadcastTick();

private slots:
    /**
     * @brief Sends the stored UDP broadcast message.
     *
     * Attempts to send the current data to all known broadcast addresses.
     * If any transmission fails, the list of addresses is updated.
     */
    void sendBroadcast();
    /**
     * @brief Updates the broadcast data with new lobby information.
     * @param lobbyInfo The new lobby information to broadcast.
     */
    void refreshLobbyInfo(const LobbyInfo &lobbyInfo);

private:
    /**
     * @brief Updates the list of available broadcast addresses.
     *
     * Scans all network interfaces to detect available broadcast addresses.
     * The list is refreshed to ensure messages are sent to valid destinations.
     */
    void updateAddresses();

    QList<QHostAddress> broadcastAddresses; ///< List of currently available broadcast addresses.

    QUdpSocket udpSocket; ///< UDP socket used for sending data.

    QTimer broadcastTimer; ///< Timer for scheduling periodic broadcasts.

    const quint16 port; ///< The port used for broadcasting messages.

    QByteArray currentData; ///< The latest lobby information to be sent in broadcasts.
};

#endif // UDPBROADCASTER_H

