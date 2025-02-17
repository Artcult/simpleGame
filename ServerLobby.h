#ifndef SERVERLOBBY_H
#define SERVERLOBBY_H

#include <QObject>
#include <memory>
#include "LanTcpServer.h"
#include "PlayerConnection.h"
#include "UdpBroadcaster.h"
#include "LobbyInfo.h"

/**
 * @brief Manages player connections, server state, and UDP broadcasts.
 */
class ServerLobby : public QObject {
    Q_OBJECT
public:
    explicit ServerLobby(int maxPlayers, quint16 serverPort, quint16 broadcastPort, QObject *parent = nullptr);

    bool startLobby();
    void stopLobby();

    void startBroadcast();
    void stopBroadcast();

    bool connectAsHost(const QString &hostAddress);
    QString getLobbyInfo() const;

signals:
    void tryConnect(const QString &address, quint16 port);
    void tryDisconnect();
    void connectResult(bool success, const QString &message);
    void broadcastStatusChanged(bool isActive);
    void hostConnectionStatus(bool success);
    void playerCountChanged(int currentPlayers);
    void lobbyInfoUpdated(const LobbyInfo &info);

private slots:
    void onPlayerConnected(const PlayerConnection &player);
    void onPlayerDisconnected(const PlayerConnection &player);
    void updateLobbyInfo();

private:
    std::unique_ptr<LanTcpServer> server;
    std::unique_ptr<UdpBroadcaster> broadcaster;
    LobbyInfo lobbyInfo;

    const int maxPlayers;
    const quint16 tcpPort;
    const quint16 udpPort;
    QList<PlayerConnection> players;

    bool isRoomAvailable() const;
    void refreshLobbyInfo();
};

#endif // SERVERLOBBY_H
