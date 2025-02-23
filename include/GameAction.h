#ifndef GAMEACTION_H
#define GAMEACTION_H

#include "INetworkSerializable.h"
#include <QDataStream>
#include <QHostAddress>

/**
 * @brief Enum representing possible game actions (Rock, Paper, Scissors).
 */
enum class ActionType {
    Rock,      ///< Rock action.
    Paper,     ///< Paper action.
    Scissors   ///< Scissors action.
};

/**
 * @brief Represents a player's action in the game, which can be serialized for network transmission.
 */
struct GameAction : public INetworkSerializable {
    ActionType action;  ///< The action chosen by the player.
    QString playerName; ///< The name of the player who performed the action.

    GameAction() = default;

    /**
     * @brief Constructs a GameAction.
     * @param act The action type.
     * @param name The player's name.
     */
    GameAction(ActionType act, const QString &name) : action(act), playerName(name) {}

    /**
     * @brief Serializes the game action into a QByteArray for network transmission.
     * @return Serialized data.
     */
    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << static_cast<int>(action) << playerName;
        return data;
    }

    /**
     * @brief Deserializes a QByteArray into a GameAction object.
     * @param data The serialized game action data.
     */
    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        int actionInt;
        in >> actionInt >> playerName;
        action = static_cast<ActionType>(actionInt);
    }
};

#endif // GAMEACTION_H
