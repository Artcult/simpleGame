#ifndef LOBBYCLIENT_H
#define LOBBYCLIENT_H

#include <QObject>
#include "ServerLobby.h"
#include "UdpBroadcastListener.h"
#include "LanTcpClient.h"

class LobbyClient : public QObject {
    Q_OBJECT
public:
    explicit LobbyClient(QObject *parent = nullptr);
    ~LobbyClient() = default;

public slots:
    void onConnectToFirstFindedServer();
    void onHostOwnLocalTcpServer();
    void onCloseGame();

private:
    static constexpr const char* LOBBY_NAME = "DefaultLobby";
    static constexpr int MAX_PLAYERS = 2;
    static constexpr quint16 SERVER_PORT = 50505;
    static constexpr quint16 BROADCAST_PORT = 50005;

    void connectToLobby(const LobbyInfo &info);

    std::unique_ptr<ServerLobby> serverLobby;
    std::unique_ptr<UdpBroadcastListener> broadcastListener;
    std::unique_ptr<LanTcpClient> client;
};

#endif // LOBBYCLIENT_H
