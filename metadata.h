#ifndef METADATA_H
#define METADATA_H
// ------------------------------------------------------------------------------------------------------------------
#include <QTcpSocket>
#include <QString>
#include <QHostAddress>
#include <QMap>
// ------------------------------------------------------------------------------------------------------------------
enum class UserStatus
{
    ACTIVE,
    MUTED
};
// ------------------------------------------------------------------------------------------------------------------
struct UserMetadata {
    QString username;
    QString channelName;
    QHostAddress host;
    uint16_t port;
    UserStatus status;
};
// ------------------------------------------------------------------------------------------------------------------
struct ChannelMetadata
{
    QString channelName;
    QString password;
};
// ------------------------------------------------------------------------------------------------------------------
inline QDataStream& operator<<(QDataStream &stream, const UserMetadata& md)
{
    stream << md.username << md.channelName << md.host << md.port << static_cast<int>(md.status);
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, UserMetadata& md)
{
    stream >> md.username >> md.channelName >> md.host >> md.port >> md.status;
    return stream;
}
inline QDataStream& operator<<(QDataStream &stream, const ChannelMetadata& md)
{
    stream << md.channelName;
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, ChannelMetadata& md)
{
    stream >> md.channelName;
    return stream;
}
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
    CHANNEL_NOT_FOUND,
    CHANNEL_WRONG_PASSWORD
};
QString statusCodeToMessage(StatusCode code)
{
    switch (code)
    {
    case StatusCode::SUCCESS: return "Erfolg.";
    case StatusCode::CHANNEL_NOT_FOUND: return "Der Channel exisitiert nicht.";
    case StatusCode::CHANNEL_WRONG_PASSWORD: return "Das Passwort ist falsch.";
    }
}
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
    if (response.message.isEmpty())
        stream << statusCodeToMessage(response.code);
    else
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
struct ChannelConnectResponse : public StatusResponse
{};
inline QDataStream& operator<<(QDataStream &stream, const ChannelConnectResponse& response)
{
    stream << QString("connect");
    stream << response.downCast();
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, ChannelConnectResponse& response)
{
    stream >> response.downCast();
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
struct ClientJoinedChannelNotification
{
    UserMetadata joinedUser;
};
inline QDataStream& operator<<(QDataStream &stream, const ClientJoinedChannelNotification& noti)
{
    stream << QString("client_joined");
    stream << noti.joinedUser;
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, ClientJoinedChannelNotification& noti)
{
    stream >> noti.joinedUser;
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct OverviewRequest
{};
inline QDataStream& operator<<(QDataStream &stream, const OverviewRequest& request)
{
    stream << QString("overview");
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, OverviewRequest& request)
{
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
struct OverviewResponse
{
    bool isConnectedToChannel;
    ChannelMetadata currentChannel;
    QList<ChannelMetadata> channels;
    QList<UserMetadata> users;
};
inline QDataStream& operator<<(QDataStream &stream, const OverviewResponse& response)
{
    stream << QString("overview");
    stream << response.isConnectedToChannel << response.currentChannel;
    stream << response.channels << response.users;
    return stream;
}
inline QDataStream& operator>>(QDataStream &stream, OverviewResponse& response)
{
    return stream;
}
// ------------------------------------------------------------------------------------------------------------------
#endif // METADATA_H
