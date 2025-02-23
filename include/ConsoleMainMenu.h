#ifndef CONSOLEMAINMENU_H
#define CONSOLEMAINMENU_H

#include "IMainMenu.h"
#include <QTextStream>

/**
 * @brief Console-based implementation of the main menu.
 *        Provides an interactive text-based interface for the user.
 */
class ConsoleMainMenu : public IMainMenu {
    Q_OBJECT

public:
    /**
     * @brief Constructs the ConsoleMainMenu.
     * @param parent Parent QObject.
     */
    explicit ConsoleMainMenu(QObject *parent = nullptr);

    /**
     * @brief Displays the console menu and processes user input.
     */
    void showMenu() override;

private:
    QTextStream in;  ///< Input stream for reading user input.
    QTextStream out; ///< Output stream for displaying menu options.
};

#endif // CONSOLEMAINMENU_H
