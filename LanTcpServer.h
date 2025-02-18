#ifndef LANTCPSERVER_H
#define LANTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "PlayerConnection.h"

class LanTcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit LanTcpServer(quint16 port, QObject *parent = nullptr);
    ~LanTcpServer();

    bool startListening();
    void stopListening();
    void stopAcceptingPlayers();
    void disconnectPlayer(const PlayerConnection &player);
    void sendMessageToAll(const QByteArray &message);
    void sendMessageToPlayer(const PlayerConnection &player, const QByteArray &message);

signals:
    void playerConnected(const PlayerConnection &player);
    void playerDisconnected(const PlayerConnection &player);
    void messageReceived(const PlayerConnection &player, const QByteArray &message);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onClientDisconnected();

private:
    quint16 serverPort;
    bool acceptingPlayers = true;

    // Сопоставляем сокеты и данные о подключенных игроках
    QMap<QTcpSocket*, PlayerConnection> clients;

    PlayerConnection createPlayerFromSocket(QTcpSocket *socket);
    void removeClient(QTcpSocket *socket);
};

#endif // LANTCPSERVER_H
