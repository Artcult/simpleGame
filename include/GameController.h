#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include "IMainMenu.h"
#include "IGameActionMenu.h"
#include "LobbyClient.h"

/**
 * @brief Controls the interaction between the main menu, game action menu, and lobby client.
 *        Manages the flow of the game, including menu navigation, player choices, and results.
 */
class GameController : public QObject {
public:
    /**
     * @brief Constructs the GameController and initializes the menu and action menu.
     * @param menu Pointer to the main menu interface.
     * @param actionMenu Pointer to the game action menu interface.
     * @param parent Parent QObject.
     */
    explicit GameController(IMainMenu *menu, IGameActionMenu *actionMenu, QObject *parent = nullptr);

    /**
     * @brief Starts the main menu.
     */
    void invokeMainMenu();

private:
    IMainMenu *mainMenu;      ///< Pointer to the main menu interface.
    IGameActionMenu *gameActionMenu; ///< Pointer to the game action menu interface.
    LobbyClient lobbyClient;  ///< Handles network communication for the game.

private slots:
    /**
     * @brief Handles connecting to the first available server.
     */
    void onConnectToFirstFindedServer();

    /**
     * @brief Handles hosting a local TCP server.
     */
    void onHostOwnLocalTcpServer();

    /**
     * @brief Handles closing the game.
     */
    void onCloseGame();

    /**
     * @brief Handles the player's move selection.
     * @param choice The player's choice (1 = Rock, 2 = Paper, 3 = Scissors).
     */
    void onPlayerMadeChoice(int choice);

    /**
     * @brief Displays the game action menu when invoked by the server.
     */
    void onInvokeGameActionMenu();

    /**
     * @brief Displays the game result when received from the server.
     * @param result The result message (Win/Lose/Draw).
     */
    void onInvokeResult(QString result);
};

#endif // GAMECONTROLLER_H
