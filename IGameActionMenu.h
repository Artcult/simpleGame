#ifndef IGAMEACTIONMENU_H
#define IGAMEACTIONMENU_H

#include <QObject>

#include <QObject>

/**
 * @brief Interface for the game choice menu, allowing the player to choose Rock, Paper, or Scissors.
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
     * @brief Signal emitted when the player makes a choice.
     * @param choice The player's choice: 1 - Rock, 2 - Paper, 3 - Scissors.
     */
    void playerMadeChoice(int choice);
    void closeGame();

public:
    /**
     * @brief Shows the choice menu (to be implemented in derived classes).
     */
    virtual void showMenu() = 0;

    /**
     * @brief Shows action result (to be implemented in derived classes).
     */
    virtual void showResult(QString result) = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~IGameActionMenu() = default;
};

#endif // IGAMEACTIONMENU_H
