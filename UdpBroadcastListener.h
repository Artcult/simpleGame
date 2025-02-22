#ifndef UDPBROADCASTLISTENER_H
#define UDPBROADCASTLISTENER_H

#include <QObject>
#include <QUdpSocket>
#include "LobbyInfo.h"

/**
 * @brief A class for listening to UDP broadcast messages within a local network.
 *
 * This class listens for incoming UDP broadcast messages on a specified port,
 * extracts lobby information from the received data, and emits a signal when
 * a new lobby is detected.
 */
class UdpBroadcastListener : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a UdpBroadcastListener.
     *
     * Initializes the UDP listener with the specified port.
     *
     * @param listenPort The port used for listening to UDP messages.
     * @param parent The parent QObject (default is nullptr).
     */
    explicit UdpBroadcastListener(quint16 listenPort, QObject *parent = nullptr);

    /**
     * @brief Starts listening for UDP broadcast messages.
     *
     * Binds the UDP socket to the specified port and connects its `readyRead`
     * signal to process incoming messages.
     */
    void startListening();

    /**
     * @brief Stops listening for UDP messages.
     *
     * Closes the UDP socket, preventing further message reception.
     */
    void stopListening();

signals:
    /**
     * @brief Emitted when a new lobby is detected.
     *
     * This signal is triggered whenever a valid lobby information packet
     * is received from a broadcast message.
     *
     * @param senderIp The IP address of the sender.
     * @param info The received lobby information.
     */
    void lobbyFound(const QHostAddress &senderIp, const LobbyInfo &info);

private slots:
    /**
     * @brief Processes incoming UDP broadcast messages.
     *
     * Reads available datagrams, extracts the sender’s IP address,
     * deserializes the lobby information, and emits the `lobbyFound` signal.
     */
    void onProcessPendingDatagrams();

private:
    QUdpSocket udpSocket; ///< UDP socket used for receiving data.

    quint16 port; ///< The port used for listening to UDP messages.
};

#endif // UDPBROADCASTLISTENER_H
