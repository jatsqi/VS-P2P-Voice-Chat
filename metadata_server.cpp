// ------------------------------------------------------------------------------------------------------------------
#include "metadata_server.h"
#include "port_discovery.h"

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
}

void CSimpleMetadataUserSocketInformation::startDiscovery()
{
    m_State = UserState::WAITING_FOR_DISCOVERY;

    m_HolePunchingServer = new CUdpHolePunchingServer(this, 18761);
    QObject::connect(m_HolePunchingServer, &CUdpHolePunchingServer::clientDiscovered, this, &CSimpleMetadataUserSocketInformation::onDiscoverySuccessful);
    m_HolePunchingServer->start();
}

void CSimpleMetadataUserSocketInformation::onSocketReadyRead()
{
    emit userSocketReadyRead();
}

void CSimpleMetadataUserSocketInformation::onDiscoverySuccessful(DiscoveryResult result)
{
    m_State = UserState::VALID;

    delete m_HolePunchingServer;
    emit discoverySuccessful(result);
}
// ------------------------------------------------------------------------------------------------------------------
CSimpleMetadataServer::CSimpleMetadataServer(QObject* parent, uint16_t port)
    : IMetadataServer(parent, port)
{
    m_Server = new QTcpServer(parent);

    QObject::connect(m_Server, &QTcpServer::newConnection, this, &CSimpleMetadataServer::onIncommingConnection);
    m_Server->listen(QHostAddress::Any, port);
}

void CSimpleMetadataServer::createChannel(QString name, QString password)
{
    ChannelMetadata metadata;
    metadata.channelName = name;
    metadata.password = password;

    NewChannelCreatedNotification noti;
    noti.channelName = name;

    sendToAllClients(noti);
}

QList<ChannelMetadata> CSimpleMetadataServer::channels() const
{
    return this->m_OwnedChannels.values();
}

void CSimpleMetadataServer::onIncommingConnection()
{
    QTcpSocket *socket = m_Server->nextPendingConnection();
    CSimpleMetadataUserSocketInformation *info = new CSimpleMetadataUserSocketInformation(this, socket);

    QObject::connect(info, &CSimpleMetadataUserSocketInformation::userSocketReadyRead, this, &CSimpleMetadataServer::userSocketReayRead);

    m_ConnectedClients.append(info);
}

void CSimpleMetadataServer::userSocketReayRead()
{
    CSimpleMetadataUserSocketInformation *info = qobject_cast<CSimpleMetadataUserSocketInformation*>(sender());

    QByteArray data = info->socket()->readAll();
    QDataStream stream(data);

    QString action;
    stream >> action;

    if (action == "identification")
    {
        IdentificationRequest identReq;
        stream >> identReq;

        info->setUsername(identReq.username);
        info->setState(CSimpleMetadataUserSocketInformation::UserState::VALID);

        IdentificationResponse identRes;
        identRes.code = StatusCode::SUCCESS;

        writeToSocket(info->socket(), identRes);
        qDebug() << "User " << identReq.username << " successfully identified!";
    }
    // Client möchte sich verbinden, starte UDP Hole Punching
    else if (action == "connect")
    {
        qDebug() << "Client wants to connect.";
        PortDiscoveryRequest portDis;
        portDis.discoveryPort = 18761;

        // Starte Hole Punching Socket
        QObject::connect(info, &CSimpleMetadataUserSocketInformation::discoverySuccessful, this, &CSimpleMetadataServer::onClientDiscoverySuccessful);
        info->startDiscovery();

        // Sende Infos an Client
        writeToSocket(info->socket(), portDis);
    }
    else
    {
        qDebug() << "Unknown Method '" << action << "'";
    }
}

void CSimpleMetadataServer::onClientDiscoverySuccessful(DiscoveryResult result)
{
    CSimpleMetadataUserSocketInformation *info = qobject_cast<CSimpleMetadataUserSocketInformation*>(sender());
    QObject::disconnect(info, &CSimpleMetadataUserSocketInformation::discoverySuccessful, this, &CSimpleMetadataServer::onClientDiscoverySuccessful);

    ChannelConnectResponse connectResponse;
    connectResponse.code = StatusCode::SUCCESS;
    writeToSocket(info->socket(), connectResponse);

    ClientJoinedChannelNotification noti;
    noti.username = info->username();
    noti.clientHost = result.address;
    noti.clientPort = result.port;
    sendToAllClients(noti);
}
// ------------------------------------------------------------------------------------------------------------------
