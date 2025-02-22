#ifndef MAINMENUCONTROLLER_H
#define MAINMENUCONTROLLER_H

#include <QObject>
#include "IMainMenu.h"
#include "LobbyClient.h"

/**
 * @brief Controls the interaction between the main menu and the lobby client.
 */
class MainMenuController : public QObject {
public:
    /**
     * @brief Constructs the MainMenuController with a given menu interface.
     * @param menu Pointer to the main menu interface.
     * @param parent Parent QObject.
     */
    explicit MainMenuController(IMainMenu *menu, QObject *parent = nullptr);

    /**
     * @brief Displays the main menu.
     */
    void invokeMainMenu();

private:
    IMainMenu *mainMenu; ///< Pointer to the main menu interface.
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
};

#endif // MAINMENUCONTROLLER_H
