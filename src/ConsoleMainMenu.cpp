#include "ConsoleMainMenu.h"

/**
 * @brief Constructs the console main menu.
 *        Initializes input and output streams.
 * @param parent Parent QObject.
 */
ConsoleMainMenu::ConsoleMainMenu(QObject *parent)
    : IMainMenu(parent), in(stdin), out(stdout) {}

/**
 * @brief Displays the console menu and handles user selection.
 *        Triggers corresponding signals based on the user's choice.
 */
void ConsoleMainMenu::showMenu() {
    // Display menu options
    out << "\n=== Menu ===\n"
        << "1. Quick Game\n"
        << "2. Host Game\n"
        << "3. Exit Game\n"
        << "Choice Option: ";
    out.flush();

    int choice;
    in >> choice; // Read user input

    switch (choice) {
    case 1:
        emit connectToFirstFindedServer(); // Signal to connect to the first available server
        return;
    case 2:
        emit hostOwnLocalTcpServer(); // Signal to host a game
        return;
    case 3:
        emit closeGame(); // Signal to close the game
        return;
    default:
        // Handle invalid input
        out << "Incorrect input\n"
            << "Try Again\n";
        out.flush();
    }
}

