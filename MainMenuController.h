#ifndef MAINMENUCONTROLLER_H
#define MAINMENUCONTROLLER_H

#include <QObject>
#include "IMainMenu.h"
#include "LobbyClient.h"

class MainMenuController : public QObject{

public:
    explicit MainMenuController(IMainMenu *menu, QObject *parent = nullptr);
    void invokeMainMenu();

private:
    IMainMenu *mainMenu;
    LobbyClient lobbyClient;

private slots:
    void onConnectToFirstFindedServer();
    void onHostOwnLocalTcpServer();
    void onCloseGame();
};

#endif // MAINMENUCONTROLLER_H
