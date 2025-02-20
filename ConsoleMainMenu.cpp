#include "ConsoleMainMenu.h"

ConsoleMainMenu::ConsoleMainMenu(QObject *parent)
    : IMainMenu(parent), in(stdin), out(stdout) {}

void ConsoleMainMenu::showMenu(){
    out << "\n=== Menu ===\n"
        << "1. Quick Game\n"
        << "2. Host Game\n"
        << "3. Exit Game\n"
        << "Choice Option: ";
    out.flush();

    int choice;
    in >> choice;

    switch (choice) {
    case 1:
        emit connectToFirstFindedServer();
        return;
    case 2:
        emit hostOwnLocalTcpServer();
        return;
    case 3:
        emit closeGame();
        return;
    default:
        out << "Bruh...\n";
        out.flush();
    }
}

