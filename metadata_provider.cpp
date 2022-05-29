#include "metadata_provider.h"

#include <QTcpSocket>
#include <QTcpServer>

MetadataBroker::MetadataBroker(QHostAddress provHost, uint16_t provPort)
    : m_ProviderHost(provHost), m_ProviderPort(provPort)
{
}

QHostAddress MetadataBroker::providerHost() const
{
    return this->m_ProviderHost;
}

uint16_t MetadataBroker::providerPort() const
{
    return this->m_ProviderPort;
}

LocalMetadataUser::LocalMetadataUser(QObject* parent, QTcpSocket *user)
    : QObject(parent), m_UserSocket(user), m_State(UserState::PENDING)
{
    QObject::connect(m_UserSocket, &QTcpSocket::readyRead, this, &LocalMetadataUser::onReadyRead);
}

void LocalMetadataUser::sendChannelsMetadata(QList<QString> channelNames)
{
    QByteArray channelsMetadata;
    QDataStream outputStream(&channelsMetadata, QIODevice::WriteOnly);
    outputStream << QString("metadata");
    outputStream << channelNames;
    m_UserSocket->write(channelsMetadata);
}

QString LocalMetadataUser::username() const
{
    return this->m_Username;
}

QString LocalMetadataUser::channel() const
{
    return this->m_Channel;
}

LocalMetadataUser::UserState LocalMetadataUser::state() const
{
    return this->m_State;
}

void LocalMetadataUser::onReadyRead()
{
    QString password;
    QString action;

    qDebug() << "Receiving data: " << m_UserSocket->bytesAvailable();

    QDataStream inputStream(m_UserSocket->readAll());
    inputStream >> action;

    if (action == "connect")
    {
        qDebug() << "trying to connect";

        inputStream >> m_Username;
        inputStream >> m_Channel;
        inputStream >> password;

        qDebug() << m_Username << " " << m_Channel << " " << password;

        emit authAttempt(m_Channel, password);
    }

    if (action == "metadata")
    {
        qDebug() << "Request metadata";
        emit metadataRequested();
    }
}

LocalMetadataBroker::LocalMetadataBroker(QHostAddress provHost, uint16_t provPort)
    : MetadataBroker(provHost, provPort)
{
    m_ProviderSocket = new QTcpSocket(this);
    m_ProviderServer = new QTcpServer(this);

    QObject::connect(m_ProviderServer, &QTcpServer::newConnection, this, &LocalMetadataBroker::onIncommingConnection);
    QObject::connect(m_ProviderServer, &QTcpServer::acceptError, this, &LocalMetadataBroker::onServerError);
    QObject::connect(m_ProviderSocket, &QTcpSocket::connected, this, &LocalMetadataBroker::onSocketConnected);
    QObject::connect(m_ProviderSocket, &QTcpSocket::errorOccurred, this, &LocalMetadataBroker::onSocketError);
    QObject::connect(m_ProviderSocket, &QTcpSocket::readyRead, this, &LocalMetadataBroker::onSocketReadyRead);

    m_ProviderServer->listen(QHostAddress::Any, DEFAULT_LISTEN_PORT);
    m_ProviderSocket->connectToHost(provHost, provPort);
}

void LocalMetadataBroker::fetchChannels()
{
    m_ProviderSocket->waitForConnected(10000);

    qDebug() << "Fetching channels...";

    QByteArray fetchChannelsMessage;
    QDataStream outputStream(&fetchChannelsMessage, QIODevice::WriteOnly);
    outputStream << QString("metadata");

    m_ProviderSocket->write(fetchChannelsMessage);
    qDebug() << "SENDING...";
}

void LocalMetadataBroker::fetchChannelSpecifics(QString name, QString password)
{

}

void LocalMetadataBroker::createChannel(QString name, QString password)
{
    ChannelMetadata metadata;
    metadata.channelName = name;
    metadata.password = password;

    m_OwnedChannels.insert(name, metadata);
}

void LocalMetadataBroker::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Error in Socket :(";
}

void LocalMetadataBroker::onServerError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Errror :(";
}

void LocalMetadataBroker::onIncommingConnection()
{
    LocalMetadataUser* user = new LocalMetadataUser(this, m_ProviderServer->nextPendingConnection());
    qDebug() << "Incomming connection!";

    QObject::connect(user, &LocalMetadataUser::authAttempt, this, &LocalMetadataBroker::onUserAuthAttempt);
    QObject::connect(user, &LocalMetadataUser::metadataRequested, this, &LocalMetadataBroker::onUserRequestsMetadata);

    m_ConnectedUsers.append(user);
}

void LocalMetadataBroker::onSocketConnected()
{
    qDebug() << "Socket connected!";
}

void LocalMetadataBroker::onSocketReadyRead()
{
    QDataStream inputStream(m_ProviderSocket->readAll());
    QString action;

    qDebug() << "Got data from provider.";
    inputStream >> action;
    qDebug() << "Action: " << action;

    if (action == "metadata")
    {
        QList<QString> channels;
        inputStream >> channels;

        qDebug() << "got " << channels.size() << " channels";
    }
}

void LocalMetadataBroker::onUserRequestsMetadata()
{
    LocalMetadataUser* user = qobject_cast<LocalMetadataUser*>(sender());

    QList<QString> names;
    for (auto &p : m_OwnedChannels)
    {
        names.append(p.channelName);
    }

    qDebug() << "Passing metadata";
    user->sendChannelsMetadata(names);
}

void LocalMetadataBroker::onUserAuthAttempt(QString channel, QString password)
{
    LocalMetadataUser* user = qobject_cast<LocalMetadataUser*>(sender());

    qDebug() << "USER TRIED TO ACCESS " << channel << " - " << user->username();

    if (m_OwnedChannels.contains(channel))
    {

    }
}

