#ifndef LOBBYCLIENT_H
#define LOBBYCLIENT_H

#include <QObject>
#include "ServerLobby.h"
#include "UdpBroadcastListener.h"
#include "LanTcpClient.h"

/**
 * @brief Manages the client's connection to game lobbies, including searching, joining, and hosting functionality.
 */
class LobbyClient : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs the LobbyClient instance.
     * @param parent The parent QObject.
     */
    explicit LobbyClient(QObject *parent = nullptr);

    /**
     * @brief Destroys the LobbyClient instance.
     */
    ~LobbyClient() = default;

signals:
    /**
     * @brief Emitted when the game action menu should be displayed.
     */
    void invokeGameActionMenu();

    /**
     * @brief Emitted when game results should be displayed.
     * @param result The result message (e.g., "You won!", "You lost!", "It's a draw!").
     */
    void invokeResults(QString result);

public slots:
    /**
     * @brief Starts searching for available lobbies and connects to the first one found.
     */
    void onConnectToFirstFindedServer();

    /**
     * @brief Hosts a local TCP server for other players to join.
     */
    void onHostOwnLocalTcpServer();

    /**
     * @brief Closes the current game session and cleans up network resources.
     */
    void onCloseGame();

    /**
     * @brief Sends the player's choice to the server.
     * @param choice The player's choice: 1 - Rock, 2 - Paper, 3 - Scissors.
     */
    void onPlayerMadeChoice(int choice);

    /**
     * @brief Handles a found lobby by attempting to connect to it.
     * @param hostAdress The IP address of the found lobby.
     * @param info Information about the found lobby.
     */
    void onLobbyFinded(const QHostAddress &hostAdress, const LobbyInfo &info);

private:
    static constexpr const char* LOBBY_NAME = "DefaultLobby";  ///< Default lobby name.
    static constexpr int MAX_PLAYERS = 2;                      ///< Maximum number of players allowed in the lobby.
    static constexpr quint16 SERVER_PORT = 50505;              ///< TCP server port for communication.
    static constexpr quint16 BROADCAST_PORT = 50005;           ///< UDP broadcast port for discovering lobbies.

    /**
     * @brief Initializes the TCP client for connecting to lobbies.
     */
    void initClient();

    std::unique_ptr<ServerLobby> serverLobby;             ///< Manages server-side lobby hosting.
    std::unique_ptr<UdpBroadcastListener> broadcastListener; ///< Listens for available lobbies via UDP broadcast.
    std::unique_ptr<LanTcpClient> client;                 ///< Handles client-side TCP connections.
};

#endif // LOBBYCLIENT_H

