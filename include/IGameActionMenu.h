#ifndef IGAMEACTIONMENU_H
#define IGAMEACTIONMENU_H

#include <QObject>

/**
 * @brief Interface for the game action menu, allowing the player to choose Rock, Paper, or Scissors.
 *        Provides methods to display the game menu and results.
 */
class IGameActionMenu : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs the IGameActionMenu interface.
     * @param parent The parent QObject.
     */
    explicit IGameActionMenu(QObject *parent = nullptr) : QObject(parent) {}

signals:
    /**
     * @brief Emitted when the player makes a choice.
     * @param choice The player's choice: 1 - Rock, 2 - Paper, 3 - Scissors.
     */
    void playerMadeChoice(int choice);

    /**
     * @brief Emitted when the game should be closed.
     */
    void closeGame();

public:
    /**
     * @brief Displays the choice menu (must be implemented in derived classes).
     */
    virtual void showMenu() = 0;

    /**
     * @brief Displays the result of the game (must be implemented in derived classes).
     * @param result The result message (e.g., "You win!", "You lose!", "It's a draw!").
     */
    virtual void showResult(QString result) = 0;

    /**
     * @brief Virtual destructor to ensure proper cleanup in derived classes.
     */
    virtual ~IGameActionMenu() = default;
};

#endif // IGAMEACTIONMENU_H
