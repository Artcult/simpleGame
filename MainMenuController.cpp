#include "MainMenuController.h"

MainMenuController::MainMenuController(IMainMenu *menu, QObject *parent)
    : QObject(parent), mainMenu(menu){

    // Connecting interface signals to controller slots
    connect(mainMenu, &IMainMenu::connectToFirstFindedServer, this, &MainMenuController::onConnectToFirstFindedServer);
    connect(mainMenu, &IMainMenu::hostOwnLocalTcpServer, this, &MainMenuController::onHostOwnLocalTcpServer);
    connect(mainMenu, &IMainMenu::closeGame, this, &MainMenuController::onCloseGame);
}

void MainMenuController::invokeMainMenu()
{
    mainMenu->showMenu();
}

void MainMenuController::onConnectToFirstFindedServer()
{
    lobbyClient.onConnectToFirstFindedServer();
}

void MainMenuController::onHostOwnLocalTcpServer()
{
    lobbyClient.onHostOwnLocalTcpServer();
}

void MainMenuController::onCloseGame()
{
    lobbyClient.onCloseGame();
    exit(0);
}
