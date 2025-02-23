#include <QCoreApplication>
#include "GameController.h"
#include "ConsoleMainMenu.h"
#include "ConsoleGameAction.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ConsoleMainMenu mainMenu;
    ConsoleGameAction gameActionMenu;
    GameController gameController(&mainMenu,&gameActionMenu);

    gameController.invokeMainMenu();

    return a.exec();
}
