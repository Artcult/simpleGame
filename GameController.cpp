#include "GameController.h"

/**
 * @brief Constructs the GameController and connects menu signals to controller slots.
 * @param menu Pointer to the main menu interface.
 * @param parent Parent QObject.
 */
GameController::GameController(IMainMenu *menu,IGameActionMenu *actionMenu,QObject *parent)
    : QObject(parent), mainMenu(menu), gameActionMenu(actionMenu) {



    // Connecting menu interface signals to corresponding controller slots
    connect(mainMenu, &IMainMenu::connectToFirstFindedServer, this, &GameController::onConnectToFirstFindedServer);
    connect(mainMenu, &IMainMenu::hostOwnLocalTcpServer, this, &GameController::onHostOwnLocalTcpServer);
    connect(mainMenu, &IMainMenu::closeGame, this, &GameController::onCloseGame);
    connect(gameActionMenu, &IGameActionMenu::playerMadeChoice,this, &GameController::onPlayerMadeChoice);
    connect(gameActionMenu, &IGameActionMenu::closeGame,this, &GameController::onCloseGame);
    connect(&lobbyClient, &LobbyClient::invokeGameActionMenu,this, &GameController::onInvokeGameActionMenu);
    connect(&lobbyClient, &LobbyClient::invokeResults,this, &GameController::onInvokeResult);
}

/**
 * @brief Displays the main menu.
 */
void GameController::invokeMainMenu() {
    mainMenu->showMenu();
}

/**
 * @brief Handles the event when connecting to the first available server.
 */
void GameController::onConnectToFirstFindedServer() {
    lobbyClient.onConnectToFirstFindedServer();
}

/**
 * @brief Handles the event when hosting a local TCP server.
 */
void GameController::onHostOwnLocalTcpServer() {
    lobbyClient.onHostOwnLocalTcpServer();
}

/**
 * @brief Handles the event when closing the game.
 */
void GameController::onCloseGame() {
    lobbyClient.onCloseGame();
    exit(0); // Terminate the application
}

void GameController::onPlayerMadeChoice(int choice){
    lobbyClient.onPlayerMadeChoice(choice);
}

void GameController::onInvokeGameActionMenu(){
    gameActionMenu->showMenu();
}

void GameController::onInvokeResult(QString result){
    gameActionMenu->showResult(result);
}
