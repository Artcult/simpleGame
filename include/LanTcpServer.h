#ifndef LANTCPSERVER_H
#define LANTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "PlayerConnection.h"

/**
 * @brief A TCP server class for managing player connections in a LAN game.
 *
 * LanTcpServer listens for incoming player connections, manages connected clients,
 * and facilitates message exchange between players.
 */
class LanTcpServer : public QTcpServer {
    Q_OBJECT
public:
    /**
     * @brief Constructs a LanTcpServer instance.
     *
     * @param port The port on which the server will listen for connections.
     * @param parent The parent QObject (default is nullptr).
     */
    explicit LanTcpServer(quint16 port, QObject *parent = nullptr);

    /**
     * @brief Destructor for LanTcpServer.
     *
     * Stops the server and disconnects all players.
     */
    ~LanTcpServer();

    /**
     * @brief Starts listening for incoming connections.
     *
     * Binds the server to the specified port and begins accepting connections.
     *
     * @return true if the server started successfully, false otherwise.
     */
    bool startListening();

    /**
     * @brief Stops the server and disconnects all players.
     */
    void stopListening();

    /**
     * @brief Enables or disables accepting new player connections.
     * @param allowNewPlayers If true, allows new players to connect; if false, stops accepting new connections.
     */
    void setAcceptingPlayers(bool allowNewPlayers);

    /**
     * @brief Disconnects a specific player from the server.
     *
     * @param player The player to be disconnected.
     */
    void disconnectPlayer(const PlayerConnection &player);

    /**
     * @brief Sends a message to all connected players.
     *
     * @param message The message to be sent.
     */
    void sendMessageToAll(const QByteArray &message);

    /**
     * @brief Sends a message to a specific player.
     *
     * @param player The recipient player.
     * @param message The message to send.
     */
    void sendMessageToPlayer(const PlayerConnection &player, const QByteArray &message);

signals:
    /**
     * @brief Emitted when a new player connects.
     * @param player The connected player.
     */
    void playerConnected(const PlayerConnection &player);

    /**
     * @brief Emitted when a player disconnects.
     * @param player The disconnected player.
     */
    void playerDisconnected(const PlayerConnection &player);

    /**
     * @brief Emitted when a message is received from a player.
     * @param player The sender of the message.
     * @param message The received message.
     */
    void messageReceived(const PlayerConnection &player, const QByteArray &message);

protected:
    /**
     * @brief Handles new incoming player connections.
     *
     * @param socketDescriptor The descriptor of the incoming connection.
     */
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    /**
     * @brief Reads incoming messages from clients.
     */
    void onReadyRead();

    /**
     * @brief Handles client disconnection events.
     */
    void onClientDisconnected();

private:
    quint16 serverPort; ///< The port on which the server listens.
    bool acceptingPlayers = true; ///< Indicates whether new players can join.

    /// Stores connected players with their sockets.
    QMap<QTcpSocket*, PlayerConnection> players;

    /**
     * @brief Creates a PlayerConnection object from a socket.
     * @param socket The player's socket.
     * @return The corresponding PlayerConnection.
     */
    PlayerConnection createPlayerFromSocket(QTcpSocket *socket);

    /**
     * @brief Removes a client from the server and cleans up resources.
     * @param socket The socket of the player to remove.
     */
    void removeClient(QTcpSocket *socket);
};

#endif // LANTCPSERVER_H
