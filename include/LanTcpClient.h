#ifndef LANTCPCLIENT_H
#define LANTCPCLIENT_H

#include <QTcpSocket>
#include <QObject>
#include "LobbyInfo.h"

/**
 * @brief The LanTcpClient class handles TCP communication with a game server.
 *
 * This class provides methods to connect to a server, send messages, and handle
 * incoming data. It also emits signals for connection status and received messages.
 */
class LanTcpClient : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a LanTcpClient instance.
     * @param parent The parent QObject (default is nullptr).
     */
    explicit LanTcpClient(QObject *parent = nullptr);

    /**
     * @brief Destructor.
     *
     * Ensures that the client disconnects from the server before destruction.
     */
    ~LanTcpClient();

    /**
     * @brief Connects to a game server using the provided address and lobby information.
     *
     * If the client is not already connected, it attempts to establish a TCP connection.
     *
     * @param hostAdress The IP address of the server.
     * @param info The lobby information containing the server's TCP port.
     */
    void connectToServer(const QHostAddress &hostAdress, const LobbyInfo &info);

    /**
     * @brief Disconnects from the currently connected server.
     *
     * If the client is connected, it sends a disconnect request to the server.
     */
    void disconnectFromServer();

    /**
     * @brief Sends a message to the connected server.
     *
     * If the client is connected, the provided message is transmitted.
     *
     * @param message The data to be sent.
     */
    void sendMessage(const QByteArray &message);

signals:
    /**
     * @brief Emitted when the client successfully connects to a server.
     */
    void connected();

    /**
     * @brief Emitted when the client disconnects from a server.
     */
    void disconnected();

    /**
     * @brief Emitted when a message is received from the server.
     * @param message The received data.
     */
    void messageReceived(const QByteArray &message);

    /**
     * @brief Emitted when a connection error occurs.
     * @param error A string describing the error.
     */
    void connectionError(const QString &error);

private slots:
    /**
     * @brief Handles successful connection events.
     *
     * Emits the `connected` signal when the client establishes a connection.
     */
    void onConnected();

    /**
     * @brief Handles disconnection events.
     *
     * Emits the `disconnected` signal when the client is disconnected.
     */
    void onDisconnected();

    /**
     * @brief Reads incoming data from the server.
     *
     * Emits the `messageReceived` signal with the received data.
     */
    void onReadyRead();

private:
    QTcpSocket *socket; ///< The TCP socket used for communication.
};

#endif // LANTCPCLIENT_H
