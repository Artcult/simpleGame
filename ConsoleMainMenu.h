#ifndef CONSOLEMAINMENU_H
#define CONSOLEMAINMENU_H

#include "IMainMenu.h"
#include <QTextStream>

class ConsoleMainMenu : public IMainMenu{
    Q_OBJECT

public:
    explicit ConsoleMainMenu(QObject *parent = nullptr);
    void showMenu() override;

private:
    QTextStream in;
    QTextStream out;
};

#endif // CONSOLEMAINMENU_H
