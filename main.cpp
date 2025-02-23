#include <QCoreApplication>
#include "GameController.h"
#include "ConsoleMainMenu.h"
#include "ConsoleGameAction.h"

/**
 * @brief Entry point of the application.
 *
 * This initializes the Qt core application and sets up the game menus and controller.
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Create instances of the main menu and game action menu.
    ConsoleMainMenu mainMenu;
    ConsoleGameAction gameActionMenu;

    // Initialize the game controller with the menus.
    GameController gameController(&mainMenu, &gameActionMenu);

    // Start the main menu.
    gameController.invokeMainMenu();

    // Execute the Qt event loop.
    return a.exec();
}

