#ifndef LOBBYCLIENT_H
#define LOBBYCLIENT_H

#include <QObject>
#include "ServerLobby.h"
#include "UdpBroadcastListener.h"
#include "LanTcpClient.h"

/**
 * @brief Manages the client's connection to lobbies and hosting functionality.
 */
class LobbyClient : public QObject {
    Q_OBJECT
public:
    explicit LobbyClient(QObject *parent = nullptr);
    ~LobbyClient() = default;

public slots:
    /**
     * @brief Starts searching for available lobbies and connects to the first found one.
     */
    void onConnectToFirstFindedServer();

    /**
     * @brief Hosts a local TCP server for other players to join.
     */
    void onHostOwnLocalTcpServer();

    /**
     * @brief Closes the current game session and stops networking.
     */
    void onCloseGame();

    /**
     * @brief Handles a found lobby by attempting to connect to it.
     * @param hostAdress The IP address of the found lobby.
     * @param info The information about the found lobby.
     */
    void onLobbyFinded(const QHostAddress &hostAdress, const LobbyInfo &info);

private:
    static constexpr const char* LOBBY_NAME = "DefaultLobby";  ///< Default lobby name.
    static constexpr int MAX_PLAYERS = 2;                      ///< Maximum number of players.
    static constexpr quint16 SERVER_PORT = 50505;              ///< TCP server port.
    static constexpr quint16 BROADCAST_PORT = 50005;           ///< UDP broadcast port.

    /**
     * @brief Initializes the client for connecting to lobbies.
     */
    void initClient();

    std::unique_ptr<ServerLobby> serverLobby;             ///< Manages server-side lobby hosting.
    std::unique_ptr<UdpBroadcastListener> broadcastListener; ///< Listens for available lobbies via UDP broadcast.
    std::unique_ptr<LanTcpClient> client;                 ///< Handles client-side TCP connections.
};

#endif // LOBBYCLIENT_H
