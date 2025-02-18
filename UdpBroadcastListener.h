#ifndef UDPBROADCASTLISTENER_H
#define UDPBROADCASTLISTENER_H

#include <QObject>
#include <QUdpSocket>
#include "LobbyInfo.h"

class UdpBroadcastListener : public QObject {
    Q_OBJECT
public:
    explicit UdpBroadcastListener(quint16 listenPort, QObject *parent = nullptr);
    ~UdpBroadcastListener() = default;

    void startListening();
    void stopListening();

signals:
    void lobbyFound(const LobbyInfo &info);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket udpSocket;
    quint16 port;
};

#endif // UDPBROADCASTLISTENER_H
