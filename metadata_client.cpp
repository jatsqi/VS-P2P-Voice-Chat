#ifndef METADATA_CLIENT_CPP
#define METADATA_CLIENT_CPP
// ------------------------------------------------------------------------------------------------------------------
#include "metadata_client.h"
#include "metadata.h"
#include "port_discovery.h"

#include <QTcpSocket>
// ------------------------------------------------------------------------------------------------------------------
IMetadataClient::IMetadataClient(QObject *parent, QString username, QHostAddress serverHost, uint16_t serverPort)
    : QObject(parent), m_Username(username), m_ServerHost(serverHost), m_ServerPort(serverPort)
{

}
// ------------------------------------------------------------------------------------------------------------------
CLanMetadataClient::CLanMetadataClient(QObject* parent, QString username, QHostAddress serverHost, uint16_t serverPort)
    : IMetadataClient(parent, username, serverHost, serverPort)
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
    ChannelConnectRequest request;
    request.channelName = channel;
    request.password = password;
    request.username = username();

    writeToSocket(m_Socket, request);
    qDebug() << "I want to join the channel bro";
}

void CLanMetadataClient::connect()
{
    m_Socket->connectToHost(host(), port());
    m_Socket->waitForConnected(10000);

    IdentificationRequest request;
    request.username = username();
    writeToSocket(m_Socket, request);
}

QList<ChannelMetadata> CLanMetadataClient::channels() const
{
    return m_CachedChannels.values();
}

ChannelMetadata CLanMetadataClient::channel(QString name) const
{
    return m_CachedChannels.value(name);
}

void CLanMetadataClient::onSocketReadyRead()
{
    QDataStream stream(m_Socket);

    QString action;
    while (m_Socket->bytesAvailable() > 0)
    {
        stream >> action;

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

            m_HolePunchClient = new CUdpHolePunchingClient(this, host(), portDisc.discoveryPort, 31313);
            m_HolePunchClient->start();
        }
        else if (action == "connect")
        {
            ChannelConnectResponse connResp;
            stream >> connResp;

            if (connResp.code == StatusCode::SUCCESS)
            {

            }
        }
        else if (action == "client_joined")
        {
            ClientJoinedChannelNotification noti;
            stream >> noti;

            if (noti.username != username())
                qDebug() << "New Client joined: " << noti.username;
            else
                qDebug() << "Server notified me that I joined, ignoring...";
        }
        else
        {
            qDebug() << "??????";
        }
    }
}

void CLanMetadataClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error in Client";
}
// ------------------------------------------------------------------------------------------------------------------
#endif // METADATA_CLIENT_CPP
