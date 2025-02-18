#ifndef SERVERLOBBY_H
#define SERVERLOBBY_H

#include <QObject>
#include <QString>
#include "PlayerConnection.h"
#include "LobbyInfo.h"
#include "LanTcpServer.h"
#include "UdpBroadcaster.h"

/**
 * @brief Manages player connections, server state, and UDP broadcasts.
 */
class ServerLobby : public QObject {
    Q_OBJECT
public:
    explicit ServerLobby(QString lobbyName,int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent = nullptr);

    bool startServer();
    void stopServer();

    void startBroadcast();
    void stopBroadcast();

    void startGame();

    bool connectAsHost();

signals:
    void lobbyInfoUpdated(const LobbyInfo &info);
    void playerConnected(const PlayerConnection &player);
    void playerDisconnected(const PlayerConnection &player);
    void gameStarting(const QList<PlayerConnection> &players);

private slots:
    void onPlayerConnected(const PlayerConnection &player);
    void onPlayerDisconnected(const PlayerConnection &player);
    void updateLobbyInfo();

private:
    std::unique_ptr<LanTcpServer> server;
    std::unique_ptr<UdpBroadcaster> broadcaster;

    const QString lobbyName;
    const int maxPlayers;
    const quint16 tcpPort;
    const quint16 udpPort;

    LobbyInfo lobbyInfo;
    QList<PlayerConnection> players;

    bool isRoomAvailable() const;
    void refreshLobbyInfo();
};

#endif // SERVERLOBBY_H
