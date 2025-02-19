#ifndef LANTCPCLIENT_H
#define LANTCPCLIENT_H

#include <QTcpSocket>
#include <QObject>
#include "LobbyInfo.h"

class LanTcpClient : public QObject {
    Q_OBJECT
public:
    explicit LanTcpClient(QObject *parent = nullptr);
    ~LanTcpClient();

    void connectToServer(const LobbyInfo &info);  // Подключиться к серверу
    void disconnectFromServer();  // Отключиться
    void sendMessage(const QByteArray &message);  // Отправить сообщение серверу

signals:
    void connected();
    void disconnected();
    void messageReceived(const QByteArray &message);
    void connectionError(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
};

#endif // LANTCPCLIENT_H
