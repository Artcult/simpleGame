#ifndef GAMEACTION_H
#define GAMEACTION_H

#include "INetworkSerializable.h"
#include <QDataStream>
#include <QHostAddress>

enum class ActionType {
    Rock,
    Paper,
    Scissors
};

struct GameAction : public INetworkSerializable {
    ActionType action;
    QString playerName;

    GameAction() = default;
    GameAction(ActionType act, const QString &name) : action(act), playerName(name) {}

    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << static_cast<int>(action) << playerName;
        return data;
    }

    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        int actionInt;
        in >> actionInt >> playerName;
        action = static_cast<ActionType>(actionInt);
    }
};

#endif // GAMEACTION_H
