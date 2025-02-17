#ifndef IMAINMENU_H
#define IMAINMENU_H

#include <QObject>

class IMainMenu : public QObject {
    Q_OBJECT
public:
    explicit IMainMenu(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void connectToFirstFindedServer();
    void hostOwnLocalTcpServer();
    void closeGame();

public:
    virtual void showMenu() = 0; // Абстрактный метод для показа меню
    virtual ~IMainMenu() = default;
};

#endif // IMAINMENU_H
