#ifndef INETWORKSERIALIZABLE_H
#define INETWORKSERIALIZABLE_H

#include <QByteArray>

/**
 * @brief Interface for serializing and deserializing objects for network transmission.
 */
class INetworkSerializable {
public:
    virtual ~INetworkSerializable() = default;

    /**
     * @brief Serializes the object into a QByteArray for network transmission.
     * @return Serialized byte array.
     */
    virtual QByteArray serialize() const = 0;

    /**
     * @brief Deserializes an object from a QByteArray.
     * @param data The serialized data.
     */
    virtual void deserialize(const QByteArray& data) = 0;
};

#endif // INETWORKSERIALIZABLE_H

