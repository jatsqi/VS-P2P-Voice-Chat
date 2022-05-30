#ifndef METADATA_CLIENT_H
#define METADATA_CLIENT_H
// ------------------------------------------------------------------------------------------------------------------
#include "metadata.h"

#include <QObject>
#include <QAbstractSocket>
// ------------------------------------------------------------------------------------------------------------------
class QTcpSocket;
class CUdpHolePunchingClient;
// ------------------------------------------------------------------------------------------------------------------
class IMetadataClient : public QObject
{
    Q_OBJECT
public:
    IMetadataClient(QObject *parent, QString username, QHostAddress serverHost, uint16_t serverPort);

    virtual void updateAvailableChannels() = 0;
    virtual void joinChannel(QString channel, QString password) = 0;
    virtual void connect() = 0;

    virtual QList<UserMetadata> usersInCurrentChannel() = 0;

    virtual ChannelMetadata* currentChannel() const = 0;
    virtual QList<ChannelMetadata> channels() const = 0;
    virtual ChannelMetadata channel(QString name) const = 0;

    QHostAddress host() const { return m_ServerHost; }
    QString username() const { return m_Username; }
    uint16_t port() const { return m_ServerPort; }

signals:
    void channelsUpdated();
    void channelUpdated(QString name);
    void channelConnected(ChannelMetadata metadata);

private:
    QString m_Username;
    QHostAddress m_ServerHost;
    uint16_t m_ServerPort;
};
// ------------------------------------------------------------------------------------------------------------------
class CLanMetadataClient : public IMetadataClient
{
    Q_OBJECT
public:
    CLanMetadataClient(QObject* parent, QString username, QHostAddress serverHost, uint16_t serverPort);

    virtual void updateAvailableChannels() override;
    virtual void joinChannel(QString channel, QString password) override;
    virtual void connect() override;

    virtual QList<UserMetadata> usersInCurrentChannel() override;

    virtual ChannelMetadata* currentChannel() const override;
    virtual QList<ChannelMetadata> channels() const override;
    virtual ChannelMetadata channel(QString name) const override;

public slots:
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

signals:
    void identificationSuccessful();
    void identificationFailed(QString reason);
    void connectionSuccessful(); // Wenn User erfolgreich Voice Channel betreten hat
    void connectionFailed(QString reason);
    void currentChannelUpdated(); // Wenn aktueller Channel aktualisiert wurde

private:
    CUdpHolePunchingClient *m_HolePunchClient;
    QTcpSocket *m_Socket;
    ChannelMetadata m_CurrentChannel;
    QMap<QString, ChannelMetadata> m_CachedChannels;
    QMap<QString, UserMetadata> m_CachedClients;
};
// ------------------------------------------------------------------------------------------------------------------
#endif // METADATA_CLIENT_H