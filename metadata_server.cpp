// ------------------------------------------------------------------------------------------------------------------
#include "metadata_server.h"

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

void CSimpleMetadataUserSocketInformation::onSocketReadyRead()
{
    emit userSocketReadyRead();
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
        identRes.success = true;

        writeToSocket(info->socket(), identRes);
        qDebug() << "User " << identReq.username << " successfully identified!";
    } else {
        qDebug() << "Unknown Method '" << action << "'";
    }
}
// ------------------------------------------------------------------------------------------------------------------
