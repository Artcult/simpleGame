#ifndef SERVERLOBBY_H
#define SERVERLOBBY_H

#include <QObject>
//#include <memory>
#include "lantcpserver.h"
#include "playerconnection.h"
#include "udpbroadcaster.h"

/**
 * @brief The ServerLobby class manages a server lobby with player connections and optional UDP broadcasting.
 */
class ServerLobby : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a ServerLobby with the specified maximum players and port.
     * @param maxPlayers Maximum number of players.
     * @param port Port number for the server.
     * @param parent Optional parent QObject.
     */
    explicit ServerLobby(int maxPlayers, quint16 port, QObject *parent = nullptr);

    /** Starts the lobby server. */
    bool startLobby();
    /** Stops the lobby server. */
    void stopLobby();

    /** Starts broadcasting server information via UDP. */
    void startBroadcast();
    /** Stops UDP broadcasting. */
    void stopBroadcast();

    /** Connects the local player as the server host. */
    bool connectAsHost(const QString &hostAddress);
    /** Returns information about the lobby. */
    QString getLobbyInfo() const;

signals:
    void tryConnect(const QString &address, quint16 port);
    void tryDisconnect();
    void connectResult(bool success, const QString &message);
    void broadcastStatusChanged(bool isActive);
    void hostConnectionStatus(bool success);
    void playerCountChanged(int currentPlayers);

private slots:
    void onPlayerConnected(const PlayerConnection &player);
    void onPlayerDisconnected(const PlayerConnection &player);
    void handleConnectionRequest(const QString &address, quint16 port);

private:
    std::unique_ptr<LanTcpServer> server;   // Use smart pointers for safe memory management
    std::unique_ptr<UdpBroadcaster> broadcaster;
    const int maxPlayers;                   // Mark constant values as const
    const quint16 port;
    QList<PlayerConnection> players;

    /** @brief Checks if room is available in the lobby. */
    bool isRoomAvailable() const;
};

#endif // SERVERLOBBY_H
