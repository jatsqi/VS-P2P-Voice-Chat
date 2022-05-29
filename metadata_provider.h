#ifndef METADATA_PROVIDER_H
#define METADATA_PROVIDER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHostAddress>
#include <QAbstractSocket>

class QTcpSocket;
class QTcpServer;

struct UserMetadata
{
    QString username;
    QHostAddress host;
    uint16_t port;
};

struct ChannelMetadata
{
    QString channelName;
    QString password;
    QMap<QString, UserMetadata> participants;
};

class MetadataBroker : public QObject
{
    Q_OBJECT
public:
    MetadataBroker(QHostAddress provHost, uint16_t provPort);

    virtual void fetchChannels() = 0;
    virtual void fetchChannelSpecifics(QString name, QString password) = 0;
    virtual void createChannel(QString name, QString password) = 0;

    QHostAddress providerHost() const;
    uint16_t providerPort() const;

private:
    QHostAddress m_ProviderHost;
    uint16_t m_ProviderPort;

signals:
    void specificChannelChanged(ChannelMetadata channel);
    void channelsChanged(QList<ChannelMetadata> channels);
    void usersChanged(QList<UserMetadata> users);
};

class LocalMetadataUser : public QObject
{
    Q_OBJECT
public:
    enum class UserState
    {
        CONNECTED,
        PENDING,
        CLOSED,
        ERROR
    };

    LocalMetadataUser(QObject* parent, QTcpSocket *user);

    void sendChannelsMetadata(QList<QString> channelNames);
    void grantAccess();
    void denyAccess();

    QString username() const;
    QString channel() const;
    UserState state() const;

public slots:
    void onReadyRead();

signals:
    void authAttempt(QString channel, QString password);
    void metadataRequested();

private:
    QString m_Username;
    QString m_Channel;

    QTcpSocket *m_UserSocket;
    UserState m_State;

signals:
    void stateChanged(UserState oldState, UserState newState);
};

class LocalMetadataBroker : public MetadataBroker
{
    Q_OBJECT
public:
    LocalMetadataBroker(QHostAddress provHost, uint16_t provPort);

    virtual void fetchChannels() override;
    virtual void fetchChannelSpecifics(QString name, QString password) override;
    virtual void createChannel(QString name, QString password) override;

public slots:
    void onServerError(QAbstractSocket::SocketError socketError);
    void onIncommingConnection();

    void onSocketError(QAbstractSocket::SocketError socketError);
    void onSocketConnected();
    void onSocketReadyRead();

    void onUserRequestsMetadata();
    void onUserAuthAttempt(QString channel, QString password);

public:
    static const uint16_t DEFAULT_LISTEN_PORT = 34561;

private:
    QTcpSocket* m_ProviderSocket;
    QTcpServer* m_ProviderServer;
    QMap<QString, ChannelMetadata> m_OwnedChannels;
    QList<LocalMetadataUser*> m_ConnectedUsers;
};

#endif // METADATA_PROVIDER_H
