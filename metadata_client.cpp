#ifndef METADATA_CLIENT_CPP
#define METADATA_CLIENT_CPP
// ------------------------------------------------------------------------------------------------------------------
#include "metadata_client.h"
#include "metadata.h"

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

    m_Socket->connectToHost(serverHost, serverPort);
    m_Socket->waitForConnected(10000);

    IdentificationRequest request;
    request.username = username;
    writeToSocket(m_Socket, request);
}

void CLanMetadataClient::updateAvailableChannels()
{
    ChannelMetadataRequest request;
    writeToSocket(m_Socket, request);
}

void CLanMetadataClient::subscribeToChannel(QString channel, QString password)
{
    ChannelConnectRequest request;
    request.channelName = channel;
    request.password = password;
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
    QByteArray data = m_Socket->readAll();
    QDataStream stream(data);

    QString action;
    stream >> action;

    if (action == "identification")
    {
        IdentificationResponse identRes;
        stream >> identRes;

        qDebug() << "Identification response received: " << identRes.success;

        if (identRes.success)
            emit identificationSuccessful();
        else
            emit identificationFailed(identRes.message);
    }
}

void CLanMetadataClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error in Client";
}
// ------------------------------------------------------------------------------------------------------------------
#endif // METADATA_CLIENT_CPP
