// ------------------------------------------------------------------------------------------------------------------
#include "metadata_server.h"
#include "port_discovery.h"
#include "random_helper.h"

#include <QTcpServer>
// ------------------------------------------------------------------------------------------------------------------
IMetadataServer::IMetadataServer(QObject* parent, uint16_t port)
    : QObject(parent), m_Port(port)
{

}
// ------------------------------------------------------------------------------------------------------------------
CSimpleMetadataUserSocketInformation::CSimpleMetadataUserSocketInformation(QObject *parent, QTcpSocket *socket)
    : QObject(parent), m_Socket(socket), m_State(UserState::WAITING_FOR_IDENTIFICATION)
{
    QObject::connect(m_Socket, &QTcpSocket::readyRead, this, &CSimpleMetadataUserSocketInformation::onSocketReadyRead);
    QObject::connect(m_Socket, &QTcpSocket::errorOccurred, this, &CSimpleMetadataUserSocketInformation::onSocketError);
}

void CSimpleMetadataUserSocketInformation::startDiscovery(uint16_t port)
{
    m_State = UserState::WAITING_FOR_DISCOVERY;
    m_LastDiscoveryResult = DiscoveryResult();

    m_HolePunchingServer = new CUdpHolePunchingServer(this, port);
    QObject::connect(m_HolePunchingServer, &CUdpHolePunchingServer::clientDiscovered, this, &CSimpleMetadataUserSocketInformation::onDiscoverySuccessful);
    m_HolePunchingServer->start();
}

void CSimpleMetadataUserSocketInformation::onSocketReadyRead()
{
    emit userSocketReadyRead();
}

void CSimpleMetadataUserSocketInformation::onSocketError(QAbstractSocket::SocketError error)
{
    emit userSocketError(error);
}

void CSimpleMetadataUserSocketInformation::onDiscoverySuccessful(DiscoveryResult result)
{
    m_State = UserState::VALID;
    m_LastDiscoveryResult = result;

    qDebug() << "Pre delete...";
    m_HolePunchingServer->deleteLater();
    m_HolePunchingServer = nullptr;
    qDebug() << "Deleted punching server...";
    emit discoverySuccessful(result);
}
// ------------------------------------------------------------------------------------------------------------------
CSimpleMetadataServer::CSimpleMetadataServer(QObject* parent, uint16_t port)
    : IMetadataServer(parent, port)
{
    m_Server = new QTcpServer(parent);
    QObject::connect(m_Server, &QTcpServer::newConnection, this, &CSimpleMetadataServer::onIncommingConnection);
    m_Server->listen(QHostAddress::Any, port);

    m_ServerHeatbeatTimer.setInterval(5000);
    m_ServerHeatbeatTimer.setSingleShot(false);
    m_ServerHeatbeatTimer.start(5000);
    QObject::connect(&m_ServerHeatbeatTimer, &QTimer::timeout, this, &CSimpleMetadataServer::onHeartbeatTimerTimeout);
}

void CSimpleMetadataServer::createChannel(QString name, QString password)
{
    ChannelMetadata metadata;
    metadata.channelName = name;
    metadata.password = password;

    NewChannelCreatedNotification noti;
    noti.channelName = name;

    m_OwnedChannels.insert(name, metadata);
    sendToAllClients(noti);
}

QList<ChannelMetadata> CSimpleMetadataServer::channels() const
{
    return this->m_OwnedChannels.values();
}

ChannelMetadata* CSimpleMetadataServer::getChannel(QString name)
{
    if (m_OwnedChannels.contains(name))
        return &m_OwnedChannels.find(name).value();

    return nullptr;
}

CSimpleMetadataUserSocketInformation* CSimpleMetadataServer::findInfoWithUsername(QString name)
{
    for (CSimpleMetadataUserSocketInformation* info : m_ConnectedClients)
    {
        if (info->username() == name)
            return info;
    }
    return nullptr;
}

void CSimpleMetadataServer::handleIdentificationAction(CSimpleMetadataUserSocketInformation *info, QDataStream &stream)
{
    IdentificationRequest identReq;
    stream >> identReq;

    IdentificationResponse identRes;
    if (findInfoWithUsername(identReq.username) == nullptr)
    {
        identRes.code = StatusCode::SUCCESS;
        info->setUsername(identReq.username);
        info->setState(CSimpleMetadataUserSocketInformation::UserState::VALID);
        qDebug() << "User " << identReq.username << " successfully identified!";
    }
    else
    {
        identRes.code = StatusCode::CLIENT_NAME_DUPLICATED;
    }

    writeToSocket(info->socket(), identRes);
    handleOverviewAction(info, stream);
}

void CSimpleMetadataServer::handleConnectAction(CSimpleMetadataUserSocketInformation *info, QDataStream &stream)
{
    qDebug() << "Client wants to connect.";
    ChannelConnectRequest connReq;
    stream >> connReq;

    ChannelMetadata* channel = getChannel(connReq.channelName);
    ChannelConnectResponse connectResponse;
    connectResponse.code = StatusCode::SUCCESS;

    if (channel != nullptr)
    {
        if (channel->password != connReq.password)
            connectResponse.code = StatusCode::CHANNEL_WRONG_PASSWORD;
    }
    else
    {
        connectResponse.code = StatusCode::CHANNEL_NOT_FOUND;
    }

    if (connectResponse.code != StatusCode::SUCCESS)
    {
        writeToSocket(info->socket(), connectResponse);
        return;
    }

    PortDiscoveryRequest portDis;
    portDis.discoveryPort = randomPort();

    // Starte Hole Punching Socket
    QObject::connect(info, &CSimpleMetadataUserSocketInformation::discoverySuccessful, this, &CSimpleMetadataServer::onClientDiscoverySuccessful);
    info->startDiscovery(portDis.discoveryPort);
    info->setChannel(connReq.channelName);

    // Sende Infos an Client
    writeToSocket(info->socket(), portDis);
}

void CSimpleMetadataServer::handleOverviewAction(CSimpleMetadataUserSocketInformation *info, QDataStream &stream)
{
    if (info->state() != CSimpleMetadataUserSocketInformation::UserState::VALID)
        return;

    OverviewResponse ovRes;
    ovRes.isConnectedToChannel = !info->channel().isEmpty() && info->state() == CSimpleMetadataUserSocketInformation::UserState::VALID;
    ovRes.channels = m_OwnedChannels.values();
    for (auto &info : m_ConnectedClients)
        ovRes.users.append(info->metadata());
    if (info->channel() != "")
        ovRes.currentChannel = *getChannel(info->channel());

    writeToSocket(info->socket(), ovRes);
}

void CSimpleMetadataServer::handleDisconnectAction(CSimpleMetadataUserSocketInformation *info, QDataStream &stream)
{
    if (info->state() != CSimpleMetadataUserSocketInformation::UserState::VALID || info->channel() == "")
        return;

    ChannelMetadata* channel = getChannel(info->channel());
    if (channel == nullptr)
        return;

    info->setChannel("");
    channel->joinedUsers.remove(info->username());

    ClientDisconnectedFromChannelNotification noti;
    noti.affectedChannel = *channel;
    noti.affectedUser = info->metadata();
    sendToAllClients(noti);
}

void CSimpleMetadataServer::onIncommingConnection()
{
    QTcpSocket *socket = m_Server->nextPendingConnection();
    CSimpleMetadataUserSocketInformation *info = new CSimpleMetadataUserSocketInformation(this, socket);

    QObject::connect(info, &CSimpleMetadataUserSocketInformation::userSocketReadyRead, this, &CSimpleMetadataServer::userSocketReayRead);
    QObject::connect(info, &CSimpleMetadataUserSocketInformation::userSocketError, this, &CSimpleMetadataServer::onUserSocketError);

    m_ConnectedClients.append(info);
}

void CSimpleMetadataServer::onUserSocketError(QAbstractSocket::SocketError error)
{
    CSimpleMetadataUserSocketInformation *info = qobject_cast<CSimpleMetadataUserSocketInformation*>(sender());
    m_ConnectedClients.removeAll(info);

    if (info->channel() != "")
    {
        ChannelMetadata* md = getChannel(info->channel());
        md->joinedUsers.remove(info->username());

        if (md != nullptr)
        {
            ClientDisconnectedFromChannelNotification noti;
            noti.affectedUser = info->metadata();
            noti.affectedChannel = *md;

            sendToAllClients(noti);
        }
    }

    info->deleteLater();
}

void CSimpleMetadataServer::userSocketReayRead()
{
    CSimpleMetadataUserSocketInformation *info = qobject_cast<CSimpleMetadataUserSocketInformation*>(sender());

    QDataStream stream(info->socket());

    while (info->socket()->bytesAvailable() > 0)
    {
        QString action;
        stream >> action;

        if (action == "identification")
            handleIdentificationAction(info, stream);
        else if (action == "connect")
            handleConnectAction(info, stream);
        else if (action == "overview")
            handleOverviewAction(info, stream);
        else if (action == "disconnect")
            handleDisconnectAction(info, stream);
    }
}

void CSimpleMetadataServer::onClientDiscoverySuccessful(DiscoveryResult result)
{
    CSimpleMetadataUserSocketInformation *info = qobject_cast<CSimpleMetadataUserSocketInformation*>(sender());
    QObject::disconnect(info, &CSimpleMetadataUserSocketInformation::discoverySuccessful, this, &CSimpleMetadataServer::onClientDiscoverySuccessful);
    qDebug() << "Disconnected objects";

    ChannelMetadata* md = getChannel(info->channel());
    md->joinedUsers.insert(info->username(), info->metadata());

    ChannelConnectResponse connectResponse;
    connectResponse.code = StatusCode::SUCCESS;
    connectResponse.connectedChannel = *md;
    writeToSocket(info->socket(), connectResponse);

    ClientJoinedChannelNotification noti;
    noti.updatedChannel = *md;
    noti.joinedUser = info->metadata();
    sendToAllClients(noti);

    qDebug() << "All data written";
}

void CSimpleMetadataServer::onHeartbeatTimerTimeout()
{
    for (auto &info : m_ConnectedClients)
    {
        info->socket()->write(QByteArray());
        qDebug() << "BEAT!";
    }
}
// ------------------------------------------------------------------------------------------------------------------
