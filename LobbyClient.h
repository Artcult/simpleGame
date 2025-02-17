#ifndef LOBBYCLIENT_H
#define LOBBYCLIENT_H

#include <QObject>

class LobbyClient : public QObject {
    Q_OBJECT

public:
    explicit LobbyClient(QObject *parent = nullptr);

public slots:
    void onConnectToFirstFindedServer();
    void onHostOwnLocalTcpServer();
    void onCloseGame();
};
#endif // LOBBYCLIENT_H
