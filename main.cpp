#include <QCoreApplication>
#include "MainMenuController.h"
#include "ConsoleMainMenu.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ConsoleMainMenu mainMenu;
    MainMenuController gameController(&mainMenu);

    gameController.invokeMainMenu();

    return a.exec();
}
