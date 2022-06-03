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
    virtual void leaveChannel() = 0;
    virtual void connect() = 0;

    virtual const ChannelMetadata* currentChannel() const = 0;
    virtual QList<ChannelMetadata> channels() const = 0;
    virtual ChannelMetadata channel(QString name) const = 0;

    QHostAddress host() const { return m_ServerHost; }
    QString username() const { return m_Username; }
    uint16_t port() const { return m_ServerPort; }

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
    virtual void leaveChannel() override;
    virtual void connect() override;

    virtual const ChannelMetadata* currentChannel() const override;
    virtual QList<ChannelMetadata> channels() const override;
    virtual ChannelMetadata channel(QString name) const override;

    uint16_t prefferedVoicePort() const;

private:
    void handleIdentificationAction(QDataStream &stream);
    void handlePortDiscoveryAction(QDataStream &stream);
    void handleConnectAction(QDataStream &stream);
    void handleClientJoinedAction(QDataStream &stream);
    void handleOverviewAction(QDataStream &stream);
    void handleDisconnectAction(QDataStream &stream);

public slots:
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

signals:
    void identificationSuccessful();
    void identificationFailed(QString reason);
    void channelsUpdated();
    void connectionSuccessful(); // Wenn User erfolgreich Voice Channel betreten hat
    void connectionFailed(QString reason);
    void currentChannelUpdated(); // Wenn aktueller Channel aktualisiert wurde
    void serverConnectionError(QAbstractSocket::SocketError socketError);

private:
    uint16_t m_PreferredVoicePort;
    CUdpHolePunchingClient *m_HolePunchClient;
    QTcpSocket *m_Socket;
    QString m_CurrentChannelName;
    QMap<QString, ChannelMetadata> m_CachedChannels;
};
// ------------------------------------------------------------------------------------------------------------------
#endif // METADATA_CLIENT_H
