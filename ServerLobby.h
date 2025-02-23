#ifndef SERVERLOBBY_H
#define SERVERLOBBY_H

#include <QObject>
#include <QString>
#include "PlayerConnection.h"
#include "LobbyInfo.h"
#include "LanTcpServer.h"
#include "UdpBroadcaster.h"

/**
 * @brief Handles the game lobby, including player connections, server management,
 *        and UDP broadcasting for lobby discovery.
 */
class ServerLobby : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new ServerLobby instance.
     * @param lobbyName The name of the lobby.
     * @param maxPlayers The maximum number of players allowed in the lobby.
     * @param serverPort The TCP port used for player connections.
     * @param broadcastPort The UDP port used for lobby discovery broadcasting.
     * @param parent The parent QObject (optional).
     */
    explicit ServerLobby(QString lobbyName, int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent = nullptr);

    /**
     * @brief Starts the TCP server for player connections.
     * @return True if the server started successfully, otherwise false.
     */
    bool startServer();

    /**
     * @brief Stops the server and clears the player list.
     */
    void stopServer();

    /**
     * @brief Resumes player search by accepting new connections and restarting the broadcast.
     * @return True if the operation was successful, otherwise false.
     */
    bool resumeLobbySearch();

    /**
     * @brief Pauses player search by stopping new connections and halting the broadcast.
     */
    void pauseLobbySearch();

    /**
     * @brief Starts broadcasting lobby information over UDP.
     */
    void startBroadcast();

    /**
     * @brief Stops the UDP broadcast.
     */
    void stopBroadcast();

    /**
     * @brief Starts the game when the lobby is full.
     */
    void startGame();

signals:
    /**
     * @brief Emitted when lobby information is updated.
     * @param info The updated lobby information.
     */
    void lobbyInfoUpdated(const LobbyInfo &info);

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
     * @brief Emitted when the game is about to start.
     * @param players The list of connected players.
     */
    void gameStarting(const QList<PlayerConnection> &players);


private slots:
    /**
     * @brief Handles a new player connection.
     * @param player The connected player.
     */
    void onPlayerConnected(const PlayerConnection &player);

    /**
     * @brief Handles player disconnection.
     * @param player The disconnected player.
     */
    void onPlayerDisconnected(const PlayerConnection &player);

    /**
     * @brief Handles incoming messages from players.
     * @param player The sender of the message.
     * @param msg The received message.
     */
    void onMessageRecived(const PlayerConnection &player, const QByteArray &msg);

private:
    std::unique_ptr<LanTcpServer> server;  ///< TCP server for managing player connections.
    std::unique_ptr<UdpBroadcaster> broadcaster;  ///< UDP broadcaster for lobby discovery.

    const QString lobbyName;  ///< The name of the lobby.
    const int maxPlayers;  ///< Maximum number of players allowed.
    const quint16 tcpPort;  ///< TCP port for player connections.
    const quint16 udpPort;  ///< UDP port for lobby broadcasting.

    LobbyInfo lobbyInfo;  ///< Current lobby information.
    QList<PlayerConnection> players;  ///< List of connected players.
    QMap<int, int> playerChoices;

    /**
     * @brief Checks if there is room for more players in the lobby.
     * @return True if space is available, otherwise false.
     */
    bool isRoomAvailable() const;

    /**
     * @brief Updates and broadcasts lobby information.
     */
    void refreshLobbyInfo();

    int getPlayerId(const PlayerConnection &player);
    void playerMove(int playerId, int choice);
    void calculateWinners();
    void sendWinnersAndLosers(const QList<int> &winners);
};

#endif // SERVERLOBBY_H
