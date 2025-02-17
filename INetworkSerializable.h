#ifndef INETWORKSERIALIZABLE_H
#define INETWORKSERIALIZABLE_H

#include <QByteArray>

class INetworkSerializable {
public:
    virtual ~INetworkSerializable() = default;
    virtual QByteArray serialize() const = 0;
    virtual void deserialize(const QByteArray& data) = 0;
};


#endif // INETWORKSERIALIZABLE_H
