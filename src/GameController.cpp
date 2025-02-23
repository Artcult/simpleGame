#include "GameController.h"

/**
 * @brief Constructs the GameController and connects menu signals to controller slots.
 * @param menu Pointer to the main menu interface.
 * @param actionMenu Pointer to the game action menu interface.
 * @param parent Parent QObject.
 */
GameController::GameController(IMainMenu *menu, IGameActionMenu *actionMenu, QObject *parent)
    : QObject(parent), mainMenu(menu), gameActionMenu(actionMenu) {

    // Connect main menu signals to corresponding controller slots
    connect(mainMenu, &IMainMenu::connectToFirstFindedServer, this, &GameController::onConnectToFirstFindedServer);
    connect(mainMenu, &IMainMenu::hostOwnLocalTcpServer, this, &GameController::onHostOwnLocalTcpServer);
    connect(mainMenu, &IMainMenu::closeGame, this, &GameController::onCloseGame);

    // Connect game action menu signals to controller slots
    connect(gameActionMenu, &IGameActionMenu::playerMadeChoice, this, &GameController::onPlayerMadeChoice);
    connect(gameActionMenu, &IGameActionMenu::closeGame, this, &GameController::onCloseGame);

    // Connect lobby client signals to controller slots
    connect(&lobbyClient, &LobbyClient::invokeGameActionMenu, this, &GameController::onInvokeGameActionMenu);
    connect(&lobbyClient, &LobbyClient::invokeResults, this, &GameController::onInvokeResult);
}

/**
 * @brief Starts the main menu.
 */
void GameController::invokeMainMenu() {
    mainMenu->showMenu();
}

/**
 * @brief Handles connecting to the first available server.
 */
void GameController::onConnectToFirstFindedServer() {
    lobbyClient.onConnectToFirstFindedServer();
}

/**
 * @brief Handles hosting a local TCP server.
 */
void GameController::onHostOwnLocalTcpServer() {
    lobbyClient.onHostOwnLocalTcpServer();
}

/**
 * @brief Handles closing the game by calling the lobby client and exiting the application.
 */
void GameController::onCloseGame() {
    lobbyClient.onCloseGame();
    exit(0); // Terminates the application
}

/**
 * @brief Sends the player's move choice to the server.
 * @param choice The player's choice (1 = Rock, 2 = Paper, 3 = Scissors).
 */
void GameController::onPlayerMadeChoice(int choice) {
    lobbyClient.onPlayerMadeChoice(choice);
}

/**
 * @brief Displays the game action menu when the server requests it.
 */
void GameController::onInvokeGameActionMenu() {
    gameActionMenu->showMenu();
}

/**
 * @brief Displays the game result when received from the server.
 * @param result The result message (Win/Lose/Draw).
 */
void GameController::onInvokeResult(QString result) {
    gameActionMenu->showResult(result);
}
