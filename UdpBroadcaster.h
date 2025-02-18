#ifndef UDPBROADCASTER_H
#define UDPBROADCASTER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "LobbyInfo.h"

class UdpBroadcaster : public QObject {
    Q_OBJECT
public:
    explicit UdpBroadcaster(quint16 broadcastPort, QObject *parent = nullptr);
    ~UdpBroadcaster() = default;

    void startBroadcast(const LobbyInfo &info);
    void stopBroadcast();

signals:
    void broadcastTick();

private slots:
    void sendBroadcast();

private:
    QUdpSocket udpSocket;
    QTimer broadcastTimer;
    quint16 port;
    LobbyInfo currentLobbyInfo;
};

#endif // UDPBROADCASTER_H

