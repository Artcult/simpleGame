#ifndef CONSOLEGAMEACTION_H
#define CONSOLEGAMEACTION_H

#include "IGameActionMenu.h"
#include <QTextStream>

/**
 * @brief Console-based implementation of the game action menu.
 *        Allows the player to choose Rock, Paper, or Scissors.
 */
class ConsoleGameAction : public IGameActionMenu {
    Q_OBJECT

public:
    /**
     * @brief Constructs the ConsoleGameAction menu.
     * @param parent Parent QObject.
     */
    explicit ConsoleGameAction(QObject *parent = nullptr);

    /**
     * @brief Displays the console menu and processes user input.
     *        Players choose Rock, Paper, or Scissors.
     */
    void showMenu() override;

    /**
     * @brief Displays the result of the game.
     *        Waits for player input before closing the game.
     * @param result The result message (Win/Lose/Draw).
     */
    void showResult(QString result) override;

private:
    QTextStream in;  ///< Input stream for reading user input.
    QTextStream out; ///< Output stream for displaying options.
};

#endif // CONSOLEGAMEACTION_H

