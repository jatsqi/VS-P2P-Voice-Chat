#ifndef METADATA_H
#define METADATA_H
// ------------------------------------------------------------------------------------------------------------------
#include <QTcpSocket>
#include <QString>
#include <QHostAddress>
#include <QMap>
// ------------------------------------------------------------------------------------------------------------------
struct UserMetadata {
    QString username;
    QHostAddress host;
    uint16_t port;
};
// ------------------------------------------------------------------------------------------------------------------
struct ChannelMetadata
{
    QString channelName;
    QString password;
};
// ------------------------------------------------------------------------------------------------------------------
template<typename T>
void writeToSocket(QTcpSocket *socket, const T& data)
{
    socket->waitForConnected(5000);

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << data;
    socket->write(array);
}
// ------------------------------------------------------------------------------------------------------------------
template<typename T>
T readFromSocket(QTcpSocket *socket)
{
    socket->waitForConnected(5000);

    T storage;
    QByteArray data = socket->readAll();
    QDataStream stream(data);

    stream >> storage;
    return storage;
}
// ------------------------------------------------------------------------------------------------------------------
enum class StatusCode
{
    SUCCESS = 0,

};
struct StatusResponse
{
    StatusCode code;
    QString message;

    StatusResponse& downCast() { return *this; }
    const StatusResponse& downCast() const { return *this; }
};
inline QDataStream& operator<<(QDataStream &stream, const StatusResponse& response)
{
    stream << static_cast<int>(response.code);
    stream << response.message;

    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, StatusResponse& response)
{
    int status;
    stream >> status;
    stream >> response.message;

    response.code = static_cast<StatusCode>(status);
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct ChannelMetadataRequest
{
};
inline QDataStream& operator<<(QDataStream &stream, const ChannelMetadataRequest& request)
{
    stream << QString("metadata");
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, ChannelMetadataRequest& request)
{
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct ChannelConnectRequest
{
    QString username;
    QString channelName;
    QString password;
};
inline QDataStream& operator<<(QDataStream &stream, const ChannelConnectRequest& request)
{
    stream << QString("connect");
    stream << request.username << request.channelName << request.password;
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, ChannelConnectRequest& request)
{
    stream >> request.username >> request.channelName >> request.password;
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct NewChannelCreatedNotification
{
    QString channelName;
};
inline QDataStream& operator<<(QDataStream &stream, const NewChannelCreatedNotification& request)
{
    stream << QString("channel_created");
    stream << request.channelName;
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, NewChannelCreatedNotification& request)
{
    stream >> request.channelName;
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct IdentificationRequest
{
    QString username;
};
inline QDataStream& operator<<(QDataStream &stream, const IdentificationRequest& request)
{
    stream << QString("identification");
    stream << request.username;
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, IdentificationRequest& request)
{
    stream >> request.username;
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct IdentificationResponse : public StatusResponse
{
};
inline QDataStream& operator<<(QDataStream &stream, const IdentificationResponse& response)
{
    stream << QString("identification");
    stream << response.downCast();
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, IdentificationResponse& response)
{
    stream >> response.downCast();
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct PortDiscoveryRequest
{
    uint16_t discoveryPort;
};
inline QDataStream& operator<<(QDataStream &stream, const PortDiscoveryRequest& request)
{
    stream << QString("port_discovery");
    stream << request.discoveryPort;
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, PortDiscoveryRequest& request)
{
    stream >> request.discoveryPort;
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
#endif // METADATA_H
