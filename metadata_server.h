#ifndef METADATA_SERVER_H
#define METADATA_SERVER_H
// ------------------------------------------------------------------------------------------------------------------
#include "metadata.h"
#include "port_discovery.h"

#include <QObject>
#include <QHostAddress>
// ------------------------------------------------------------------------------------------------------------------
class QTcpServer;
class CUdpHolePunchingServer;
// ------------------------------------------------------------------------------------------------------------------
class IMetadataServer : public QObject
{
    Q_OBJECT
public:
    IMetadataServer(QObject* parent, uint16_t port);

    virtual void createChannel(QString name, QString password) = 0;
    virtual QList<ChannelMetadata> channels() const = 0;

private:
    uint16_t m_Port;
};
// ------------------------------------------------------------------------------------------------------------------
class CSimpleMetadataUserSocketInformation : public QObject
{
    Q_OBJECT
public:
    enum class UserState
    {
        VALID,
        WAITING_FOR_DISCOVERY,
        WAITING_FOR_IDENTIFICATION
    };

    CSimpleMetadataUserSocketInformation(QObject* parent, QTcpSocket *socket);

    QTcpSocket* socket() { return m_Socket; }
    QString username() const { return m_Username; }
    QString channel() const { return m_Channel; }
    UserState state() const { return m_State; }

    void setUsername(QString username) { this->m_Username = username; }
    void setState(UserState state) { this->m_State = state; }
    void setChannel(QString channel) { this->m_Channel = channel; }
    UserMetadata metadata() const
    {
        UserMetadata md;
        md.username = m_Username;
        md.channelName = m_Channel;
        md.host = m_LastDiscoveryResult.address;
        md.port = m_LastDiscoveryResult.port;

        return md;
    }

    void startDiscovery();

public slots:
    void onSocketReadyRead();
    void onDiscoverySuccessful(DiscoveryResult result);

signals:
    void userSocketReadyRead();
    void discoverySuccessful(DiscoveryResult result);

private:
    CUdpHolePunchingServer* m_HolePunchingServer;
    QTcpSocket *m_Socket;
    QString m_Username;
    QString m_Channel;
    UserState m_State;
    DiscoveryResult m_LastDiscoveryResult;
};
// ------------------------------------------------------------------------------------------------------------------
class CSimpleMetadataServer : public IMetadataServer
{
    Q_OBJECT
public:
    CSimpleMetadataServer(QObject* parent, uint16_t port);

    virtual void createChannel(QString name, QString password);
    virtual QList<ChannelMetadata> channels() const;

    ChannelMetadata* getChannel(QString name)
    {
        if (m_OwnedChannels.contains(name))
            return &m_OwnedChannels.find(name).value();

        return nullptr;
    }

private:
    template<typename T>
    void sendToAllClients(T &data)
    {
        for (auto &socket : m_ConnectedClients)
        {
            if (socket->state() == CSimpleMetadataUserSocketInformation::UserState::VALID)
                writeToSocket(socket->socket(), data);
        }
    }

    template<typename T>
    void sendToAllClientsInChannel(T &data, QString channel)
    {
        for (auto &socket : m_ConnectedClients)
        {
            if (socket->state() == CSimpleMetadataUserSocketInformation::UserState::VALID &&
                    socket->channel() == channel)
                writeToSocket(socket->socket(), data);
        }
    }

public slots:
    void onIncommingConnection();
    void userSocketReayRead();
    void onClientDiscoverySuccessful(DiscoveryResult result);

private:
    QTcpServer* m_Server;
    QList<CSimpleMetadataUserSocketInformation*> m_ConnectedClients;
    QMap<QString, ChannelMetadata> m_OwnedChannels;
};
// ------------------------------------------------------------------------------------------------------------------
#endif
