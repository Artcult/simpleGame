#include "UdpBroadcaster.h"
#include <QDataStream>
#include <QNetworkDatagram>
#include <QDebug>
#include <QNetworkInterface>
#include <QUuid>

/**
 * @brief Initializes the UDP broadcaster for periodic message broadcasting.
 *
 * This constructor sets up the UDP broadcaster by:
 * - Retrieving the list of available broadcast addresses.
 * - Connecting a timer to periodically send broadcast messages.
 *
 * @param broadcastPort The port used for broadcasting messages.
 * @param parent The parent QObject (default is nullptr).
 */
UdpBroadcaster::UdpBroadcaster(const quint16 broadcastPort, QObject *parent)
    : QObject(parent), port(broadcastPort) {
    updateAddresses();
    connect(&broadcastTimer, &QTimer::timeout, this, &UdpBroadcaster::onSendBroadcast);
}

/**
 * @brief Updates the list of available broadcast addresses.
 *
 * Scans the network interfaces to detect valid broadcast addresses.
 * This ensures messages are sent only to active network destinations.
 */
void UdpBroadcaster::updateAddresses() {
    QList<QHostAddress> newBroadcastAddresses;

    // Iterate through all available network interfaces
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const auto &interface : interfaces) {
        // Iterate through all address entries of the interface
        const QList<QNetworkAddressEntry> entries = interface.addressEntries();
        for (const auto &entry : entries) {
            QHostAddress broadcastAddress = entry.broadcast();

            // Add valid broadcast addresses, excluding localhost
            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost) {
                newBroadcastAddresses << broadcastAddress;
            }
        }
    }

    // Replace the old list with the new one
    broadcastAddresses = std::move(newBroadcastAddresses);
}

/**
 * @brief Sends the stored UDP broadcast message.
 *
 * Attempts to transmit the current lobby data to all detected broadcast addresses.
 * If sending fails for any address, the list of broadcast addresses is updated.
 */
void UdpBroadcaster::onSendBroadcast() {
    bool validBroadcastAddresses = true;

    // Iterate through all known broadcast addresses and send data
    for (const auto &address : std::as_const(broadcastAddresses)) {
        if (udpSocket.writeDatagram(currentData, address, port) == -1) {
            validBroadcastAddresses = false;
        }
    }

    // If any transmission failed, refresh the list of broadcast addresses
    if (!validBroadcastAddresses) {
        updateAddresses();
    }
}

/**
 * @brief Updates the broadcast data and restarts the timer.
 * @param  lobbyInfo The new lobby information to be broadcasted.
 */
void UdpBroadcaster::onRefreshLobbyInfo(const LobbyInfo &lobbyInfo) {
    broadcastTimer.stop();  // Остановить отправку, пока обновляем данные
    currentData = lobbyInfo.serialize();
    broadcastTimer.start(1000);  // Перезапустить отправку
}

/**
 * @brief Starts broadcasting lobby information.
 *
 * Stores the provided lobby information and starts a timer to periodically
 * send the data over the network.
 *
 * @param lobbyInfo The lobby information to be broadcasted.
 */
void UdpBroadcaster::startBroadcast(const LobbyInfo &lobbyInfo) {
    currentData = lobbyInfo.serialize();

    // Start the timer if it's not already running
    if (!broadcastTimer.isActive()) {
        broadcastTimer.start(1000); // Send messages every second
    }
}

/**
 * @brief Stops broadcasting messages.
 *
 * Stops the timer responsible for sending periodic broadcasts.
 * This prevents further transmissions until restarted.
 */
void UdpBroadcaster::stopBroadcast() {
    broadcastTimer.stop();
}
