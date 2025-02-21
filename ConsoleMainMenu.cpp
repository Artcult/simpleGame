#include "ConsoleMainMenu.h"

// #define RESET   "\033[0m"
// #define RED     "\033[31m"
// #define GREEN   "\033[32m"
// #define YELLOW  "\033[33m"
// #define BLUE    "\033[34m"
// #define CYAN    "\033[36m"
// #define BOLD    "\033[1m"

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
        out << "Incorrect input\n"
            << "Try Again\n";
        out.flush();
    }
}

