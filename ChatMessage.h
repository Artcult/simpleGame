#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostAddress>

struct ChatMessage : public INetworkSerializable {
    QString sender;
    QString message;

    ChatMessage() = default;
    ChatMessage(const QString &from, const QString &msg) : sender(from), message(msg) {}

    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << sender << message;
        return data;
    }

    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        in >> sender >> message;
    }
};

#endif // CHATMESSAGE_H
