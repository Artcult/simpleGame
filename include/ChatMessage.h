#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "INetworkSerializable.h"
#include <QString>
#include <QDataStream>
#include <QHostAddress>

/**
 * @brief Represents a chat message that can be serialized and transmitted over the network.
 */
struct ChatMessage : public INetworkSerializable {
    QString sender;  ///< The name of the message sender.
    QString message; ///< The message content.

    ChatMessage() = default;

    /**
     * @brief Constructs a chat message.
     * @param from The sender's name.
     * @param msg The message text.
     */
    ChatMessage(const QString &from, const QString &msg) : sender(from), message(msg) {}

    /**
     * @brief Serializes the chat message into a QByteArray for network transmission.
     * @return Serialized data.
     */
    QByteArray serialize() const override {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << sender << message;
        return data;
    }

    /**
     * @brief Deserializes a QByteArray into a ChatMessage object.
     * @param data The serialized message data.
     */
    void deserialize(const QByteArray &data) override {
        QDataStream in(data);
        in >> sender >> message;
    }
};

#endif // CHATMESSAGE_H
