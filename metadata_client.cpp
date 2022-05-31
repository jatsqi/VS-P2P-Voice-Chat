#ifndef METADATA_CLIENT_CPP
#define METADATA_CLIENT_CPP
// ------------------------------------------------------------------------------------------------------------------
#include "metadata_client.h"
#include "metadata.h"
#include "port_discovery.h"
#include "random_helper.h"

#include <QTcpSocket>
// ------------------------------------------------------------------------------------------------------------------
IMetadataClient::IMetadataClient(QObject *parent, QString username, QHostAddress serverHost, uint16_t serverPort)
    : QObject(parent), m_Username(username), m_ServerHost(serverHost), m_ServerPort(serverPort)
{
    qDebug() << "INIT CLIENT WITH USERNAME " << username;
}
// ------------------------------------------------------------------------------------------------------------------
CLanMetadataClient::CLanMetadataClient(QObject* parent, QString username, QHostAddress serverHost, uint16_t serverPort)
    : IMetadataClient(parent, username, serverHost, serverPort), m_PreferredVoicePort(35006), m_HolePunchClient(nullptr)
{
    m_Socket = new QTcpSocket(parent);

    QObject::connect(m_Socket, &QTcpSocket::readyRead, this, &CLanMetadataClient::onSocketReadyRead);
    QObject::connect(m_Socket, &QTcpSocket::errorOccurred, this, &CLanMetadataClient::onSocketError);
}

void CLanMetadataClient::updateAvailableChannels()
{
    ChannelMetadataRequest request;
    writeToSocket(m_Socket, request);
}

void CLanMetadataClient::joinChannel(QString channel, QString password)
{
    leaveChannel();

    ChannelConnectRequest request;
    request.channelName = channel;
    request.password = password;
    request.username = username();

    writeToSocket(m_Socket, request);
    qDebug() << "I want to join the channel bro";
}

void CLanMetadataClient::leaveChannel()
{
    if (m_CurrentChannelName == "")
        return;

    ChannelDisconnectRequest request;
    writeToSocket(m_Socket, request);
}

void CLanMetadataClient::connect()
{
    m_Socket->connectToHost(host(), port());
    m_Socket->waitForConnected(10000);

    IdentificationRequest request;
    request.username = username();
    writeToSocket(m_Socket, request);
}

const ChannelMetadata* CLanMetadataClient::currentChannel() const
{
    if (m_CurrentChannelName == "")
        return nullptr;

    auto it = m_CachedChannels.find(m_CurrentChannelName);
    if (it == m_CachedChannels.end())
        return nullptr;

    return &it.value();
}

QList<ChannelMetadata> CLanMetadataClient::channels() const
{
    return m_CachedChannels.values();
}

ChannelMetadata CLanMetadataClient::channel(QString name) const
{
    return m_CachedChannels.value(name);
}

uint16_t CLanMetadataClient::prefferedVoicePort() const
{
    return m_PreferredVoicePort;
}

void CLanMetadataClient::onSocketReadyRead()
{
    qDebug() << "Socket is ready read on client.";
    QDataStream stream(m_Socket);

    QString action;
    while (m_Socket->bytesAvailable() > 0)
    {
        stream >> action;
        qDebug() << "Executing action on client " << action;

        if (action == "identification")
        {
            IdentificationResponse identRes;
            stream >> identRes;

            qDebug() << "Identification response received: " << static_cast<int>(identRes.code);

            if (identRes.code == StatusCode::SUCCESS)
                emit identificationSuccessful();
            else
                emit identificationFailed(identRes.message);
        }
        else if (action == "port_discovery")
        {
            PortDiscoveryRequest portDisc;
            stream >> portDisc;

            uint16_t localPunchingBindPort = randomPort();
            m_PreferredVoicePort = localPunchingBindPort;
            m_HolePunchClient = new CUdpHolePunchingClient(this, host(), portDisc.discoveryPort, localPunchingBindPort);
            m_HolePunchClient->start();
        }
        else if (action == "connect")
        {
            delete m_HolePunchClient;

            ChannelConnectResponse connResp;
            stream >> connResp;

            if (connResp.code == StatusCode::SUCCESS)
            {
                m_CachedChannels.insert(connResp.connectedChannel.channelName, connResp.connectedChannel);
                m_CurrentChannelName = connResp.connectedChannel.channelName;
                emit connectionSuccessful();
            }
            else
            {
                emit connectionFailed(connResp.message);
            }
        }
        else if (action == "client_joined")
        {
            ClientJoinedChannelNotification noti;
            stream >> noti;

            m_CachedChannels.insert(noti.updatedChannel.channelName, noti.updatedChannel);

            if (noti.joinedUser.username != username())
            {
                qDebug() << "New user joined: " << noti.joinedUser.username;
            }
            else
            {
                qDebug() << "Server notified me that I joined, ignoring...";
            }

            qDebug() << "Because a client joined, this is the complete list: ";
            for (const auto& a : noti.updatedChannel.joinedUsers) {
                qDebug() << "    " << a.username;
            }

            emit currentChannelUpdated();
            emit channelsUpdated();
        }
        else if (action == "overview")
        {
            qDebug() << "Overview received.";
            OverviewResponse ovRes;
            stream >> ovRes;

            m_CachedChannels.clear();
            for (const ChannelMetadata &md : ovRes.channels)
            {
                qDebug() << "Found channel: " << md.channelName;
                m_CachedChannels.insert(md.channelName, md);
            }

            emit channelsUpdated();
        }
        else if (action == "disconnect")
        {
            ClientDisconnectedFromChannelNotification noti;
            stream >> noti;

            if (noti.affectedUser.username == username())
                m_CurrentChannelName = QString();

            m_CachedChannels.insert(noti.affectedChannel.channelName, noti.affectedChannel);
            emit currentChannelUpdated();
            emit channelsUpdated();
        }
        else
        {
            qDebug() << "Unbekannte Aktion empfangen: " << action;
        }
    }
}

void CLanMetadataClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error in Client";
}
// ------------------------------------------------------------------------------------------------------------------
#endif // METADATA_CLIENT_CPP
