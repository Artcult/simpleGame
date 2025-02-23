#include "ConsoleGameAction.h"

/**
 * @brief Constructs the ConsoleGameAction.
 *        Initializes input and output streams.
 * @param parent Parent QObject.
 */
ConsoleGameAction::ConsoleGameAction(QObject *parent)
    : IGameActionMenu(parent), in(stdin), out(stdout) {}

/**
 * @brief Displays the console menu and handles user selection.
 *        Triggers the corresponding signal based on the player's choice.
 */
void ConsoleGameAction::showMenu() {
    while (true) {
        // Display options
        out << "\n=== Choose your move ===\n"
            << "1. Rock\n"
            << "2. Paper\n"
            << "3. Scissors\n"
            << "Choice Option: ";
        out.flush();

        int choice;
        in >> choice; // Read user input

        if (choice >= 1 && choice <= 3) {
            // Output player's choice
            QString moveName;
            switch (choice) {
            case 1: moveName = "Rock"; break;
            case 2: moveName = "Paper"; break;
            case 3: moveName = "Scissors"; break;
            }

            out << "You chose: " << moveName << "\n";
            out.flush();

            emit playerMadeChoice(choice); // Emit signal with player's choice
            return;
        } else {
            out << "Invalid choice. Please enter 1, 2, or 3.\n";
            out.flush();
        }
    }
}

void ConsoleGameAction::showResult(QString result) {
    out << result << "\n";
    out << "Thank you for playing!\nPress Enter to exit...\n";
    out.flush();

    in.readLine();
    in.readLine();

    emit closeGame(); // Завершаем игру
}
