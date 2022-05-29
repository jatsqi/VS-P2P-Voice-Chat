#ifndef PORT_DISCOVERY_H
#define NAT_DISCOVERY_H
// ------------------------------------------------------------------------------------------------------------------
#include <QObject>
#include <QHostAddress>
// ------------------------------------------------------------------------------------------------------------------
class QUdpSocket;
// ------------------------------------------------------------------------------------------------------------------
struct DiscoveryResult
{
    QHostAddress address;
    uint16_t port;
};
// ------------------------------------------------------------------------------------------------------------------
class INATTraversalServer : public QObject
{
    Q_OBJECT
public:
    INATTraversalServer(QObject *parent, uint16_t listenPort) : QObject(parent), m_ListenPort(listenPort) {}

    virtual void start() = 0;

    uint16_t port() const { return this->m_ListenPort; }

signals:
    void clientDiscovered(DiscoveryResult result);
    // void discoveryTimeout();

private:
    uint16_t m_ListenPort;
};
// ------------------------------------------------------------------------------------------------------------------
class CUdpHolePunchingServer : public INATTraversalServer
{
    Q_OBJECT
public:
    CUdpHolePunchingServer(QObject *parent, uint16_t listenPort) : INATTraversalServer(parent, listenPort) {}

    virtual void start() override;

public slots:
    void onSocketReadyRead();

private:
    QUdpSocket *m_Socket;
};
// ------------------------------------------------------------------------------------------------------------------
class INATTraversalClient : public QObject
{
    Q_OBJECT
public:
    INATTraversalClient(QObject *parent, QHostAddress destHost, uint16_t destPort)
        : QObject(parent), m_DestinationHost(destHost), m_DestinationPort(destPort) {}

    virtual void start() = 0;

    uint16_t port() const { return m_DestinationPort; }
    QHostAddress host() const { return m_DestinationHost; }

private:
    QHostAddress m_DestinationHost;
    uint16_t m_DestinationPort;
};
// ------------------------------------------------------------------------------------------------------------------
class CUdpHolePunchingClient : public INATTraversalClient
{
    Q_OBJECT
public:
    CUdpHolePunchingClient(QObject *parent, QHostAddress destHost, uint16_t destPort)
        : INATTraversalClient(parent, destHost, destPort) {}

    virtual void start() override;

private:
    QUdpSocket* m_Socket;
};
// ------------------------------------------------------------------------------------------------------------------
#endif // NAT_DISCOVERY_H
