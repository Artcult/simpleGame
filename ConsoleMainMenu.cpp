#include "ConsoleMainMenu.h"

ConsoleMainMenu::ConsoleMainMenu(QObject *parent)
    : IMainMenu(parent), in(stdin), out(stdout) {}

void ConsoleMainMenu::showMenu(){
    out << "\n=== Меню ===\n"
        << "1. Быстрая игра\n"
        << "2. Захостить сервер\n"
        << "3. Выйти\n"
        << "Выберите опцию: ";
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
        out << "Некорректный выбор. Попробуйте снова.\n";
        out.flush();
    }
}

