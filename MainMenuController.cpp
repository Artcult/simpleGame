#include "MainMenuController.h"

/**
 * @brief Constructs the MainMenuController and connects menu signals to controller slots.
 * @param menu Pointer to the main menu interface.
 * @param parent Parent QObject.
 */
MainMenuController::MainMenuController(IMainMenu *menu, QObject *parent)
    : QObject(parent), mainMenu(menu) {

    // Connecting menu interface signals to corresponding controller slots
    connect(mainMenu, &IMainMenu::connectToFirstFindedServer, this, &MainMenuController::onConnectToFirstFindedServer);
    connect(mainMenu, &IMainMenu::hostOwnLocalTcpServer, this, &MainMenuController::onHostOwnLocalTcpServer);
    connect(mainMenu, &IMainMenu::closeGame, this, &MainMenuController::onCloseGame);
}

/**
 * @brief Displays the main menu.
 */
void MainMenuController::invokeMainMenu() {
    mainMenu->showMenu();
}

/**
 * @brief Handles the event when connecting to the first available server.
 */
void MainMenuController::onConnectToFirstFindedServer() {
    lobbyClient.onConnectToFirstFindedServer();
}

/**
 * @brief Handles the event when hosting a local TCP server.
 */
void MainMenuController::onHostOwnLocalTcpServer() {
    lobbyClient.onHostOwnLocalTcpServer();
}

/**
 * @brief Handles the event when closing the game.
 */
void MainMenuController::onCloseGame() {
    lobbyClient.onCloseGame();
    exit(0); // Terminate the application
}
