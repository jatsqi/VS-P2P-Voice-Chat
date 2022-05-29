#ifndef NAT_DISCOVERY_H
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
    INATTraversalServer(QObject *parent, uint16_t bindPort) : QObject(parent), m_BindPort(bindPort) {}

    virtual void start() = 0;

    uint16_t port() const { return this->m_BindPort; }

signals:
    void clientDiscovered(DiscoveryResult result);
    // void discoveryTimeout();

private:
    uint16_t m_BindPort;
};
// ------------------------------------------------------------------------------------------------------------------
class CUdpHolePunchingServer : public INATTraversalServer
{
    Q_OBJECT
public:
    CUdpHolePunchingServer(QObject *parent, uint16_t bindPort) : INATTraversalServer(parent, bindPort) {}

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
    INATTraversalClient(QObject *parent, QHostAddress destHost, uint16_t destPort, uint16_t bindPort)
        : QObject(parent), m_DestinationHost(destHost), m_DestinationPort(destPort), m_BindPort(bindPort) {}

    virtual void start() = 0;

    uint16_t destinationPort() const { return m_DestinationPort; }
    uint16_t bindPort() const { return m_BindPort; }
    QHostAddress host() const { return m_DestinationHost; }

signals:
    void serverResponded();

private:
    QHostAddress m_DestinationHost;
    uint16_t m_DestinationPort;
    uint16_t m_BindPort;
};
// ------------------------------------------------------------------------------------------------------------------
class CUdpHolePunchingClient : public INATTraversalClient
{
    Q_OBJECT
public:
    CUdpHolePunchingClient(QObject *parent, QHostAddress destHost, uint16_t destPort, uint16_t bindPort)
        : INATTraversalClient(parent, destHost, destPort, bindPort) {}

    virtual void start() override;

public slots:
    void onSocketReadyRead();

private:
    QUdpSocket* m_Socket;
};
// ------------------------------------------------------------------------------------------------------------------
#endif // NAT_DISCOVERY_H
