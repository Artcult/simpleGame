#ifndef IMAINMENU_H
#define IMAINMENU_H

#include <QObject>

/**
 * @brief Interface for the main menu, providing basic functionality to start and stop the game.
 */
class IMainMenu : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs the IMainMenu interface.
     * @param parent The parent QObject.
     */
    explicit IMainMenu(QObject *parent = nullptr) : QObject(parent) {}

signals:
    /**
     * @brief Signal to connect to the first available server.
     */
    void connectToFirstFindedServer();

    /**
     * @brief Signal to host a local TCP server.
     */
    void hostOwnLocalTcpServer();

    /**
     * @brief Signal to close the game.
     */
    void closeGame();

public:
    /**
     * @brief Shows the main menu (to be implemented in derived classes).
     */
    virtual void showMenu() = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~IMainMenu() = default;
};

#endif // IMAINMENU_H

