#ifndef SERVERLOBBY_H
#define SERVERLOBBY_H

#include <QObject>
#include <QString>
#include "PlayerConnection.h"
#include "LobbyInfo.h"
#include "LanTcpServer.h"
#include "UdpBroadcaster.h"

/**
 * @brief Manages the game lobby, including player connections, server operations,
 *        and UDP broadcasting for lobby discovery.
 */
class ServerLobby : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new ServerLobby instance.
     * @param lobbyName The name of the lobby.
     * @param maxPlayers The maximum number of players allowed in the lobby.
     * @param serverPort The TCP port for player connections.
     * @param broadcastPort The UDP port for broadcasting lobby availability.
     * @param parent The parent QObject (optional).
     */
    explicit ServerLobby(QString lobbyName, int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent = nullptr);

    /**
     * @brief Starts the TCP server to accept player connections.
     * @return True if the server started successfully, otherwise false.
     */
    bool startServer();

    /**
     * @brief Stops the server and clears the player list.
     */
    void stopServer();

    /**
     * @brief Resumes the search for players by allowing new connections and restarting the broadcast.
     * @return True if successful, otherwise false.
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
     * @brief Processes incoming messages from players.
     * @param player The sender of the message.
     * @param msg The received message.
     */
    void onMessageRecived(const PlayerConnection &player, const QByteArray &msg);

private:
    std::unique_ptr<LanTcpServer> server;  ///< TCP server that manages player connections.
    std::unique_ptr<UdpBroadcaster> broadcaster;  ///< UDP broadcaster for lobby discovery.

    const QString lobbyName;  ///< The name of the lobby.
    const int maxPlayers;  ///< Maximum number of players allowed in the lobby.
    const quint16 tcpPort;  ///< TCP port used for player connections.
    const quint16 udpPort;  ///< UDP port used for broadcasting lobby information.

    LobbyInfo lobbyInfo;  ///< Stores the current lobby information.
    QList<PlayerConnection> players;  ///< List of currently connected players.
    QMap<int, int> playerChoices; ///< Stores player choices in the game.

    /**
     * @brief Checks if there is space available in the lobby.
     * @return True if there is room, otherwise false.
     */
    bool isRoomAvailable() const;

    /**
     * @brief Updates and broadcasts lobby information.
     */
    void refreshLobbyInfo();

    /**
     * @brief Retrieves a player's unique ID based on their connection information.
     * @param player The player whose ID needs to be determined.
     * @return The player's ID, or -1 if not found.
     */
    int getPlayerId(const PlayerConnection &player);

    /**
     * @brief Registers the player's move in the game.
     * @param playerId The player's ID.
     * @param choice The player's selected move (1 = Rock, 2 = Paper, 3 = Scissors).
     */
    void playerMove(int playerId, int choice);

    /**
     * @brief Determines the winners of the game based on player choices.
     */
    void calculateWinners();

    /**
     * @brief Sends game results (win/loss/draw) to players.
     * @param winners A list of player IDs who won the round.
     */
    void sendWinnersAndLosers(const QList<int> &winners);
};

#endif // SERVERLOBBY_H
