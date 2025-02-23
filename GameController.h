#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include "IMainMenu.h"
#include "IGameActionMenu.h"
#include "LobbyClient.h"

/**
 * @brief Controls the interaction between the main menu and the lobby client.
 */
class GameController : public QObject {
public:
    /**
     * @brief Constructs the GameController with a given menu interface.
     * @param menu Pointer to the main menu interface.
     * @param parent Parent QObject.
     */
    explicit GameController(IMainMenu *menu,IGameActionMenu *actionMenu, QObject *parent = nullptr);

    /**
     * @brief Displays the main menu.
     */
    void invokeMainMenu();

private:
    IMainMenu *mainMenu; ///< Pointer to the main menu interface.
    IGameActionMenu *gameActionMenu;
    LobbyClient lobbyClient; ///< Handles network connections for the game.

private slots:
    /**
     * @brief Handles the event when connecting to the first found server.
     */
    void onConnectToFirstFindedServer();

    /**
     * @brief Handles the event when hosting a local TCP server.
     */
    void onHostOwnLocalTcpServer();

    /**
     * @brief Handles the event when closing the game.
     */
    void onCloseGame();

    void onPlayerMadeChoice(int choice);

    void onInvokeGameActionMenu();

    void onInvokeResult(QString result);
};

#endif // GAMECONTROLLER_H
