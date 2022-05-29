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
    UserState state() const { return m_State; }

    void setUsername(QString username) { this->m_Username = username; }
    void setState(UserState state) { this->m_State = state; }

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
    UserState m_State;
};
// ------------------------------------------------------------------------------------------------------------------
class CSimpleMetadataServer : public IMetadataServer
{
    Q_OBJECT
public:
    CSimpleMetadataServer(QObject* parent, uint16_t port);

    virtual void createChannel(QString name, QString password);
    virtual QList<ChannelMetadata> channels() const;

private:
    template<typename T>
    void sendToAllClients(T &data)
    {
        for (auto &socket : m_ConnectedClients)
        {
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
