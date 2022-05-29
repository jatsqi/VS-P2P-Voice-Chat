// ------------------------------------------------------------------------------------------------------------------
#include "port_discovery.h"

#include <QUdpSocket>
#include <QNetworkDatagram>
// ------------------------------------------------------------------------------------------------------------------
void CUdpHolePunchingServer::start()
{
    m_Socket = new QUdpSocket(this);
    m_Socket->bind(port());

    QObject::connect(m_Socket, &QUdpSocket::readyRead, this, &CUdpHolePunchingServer::onSocketReadyRead);
}

void CUdpHolePunchingServer::onSocketReadyRead()
{
    if (m_Socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_Socket->receiveDatagram();

        DiscoveryResult result;
        result.address = datagram.senderAddress();
        result.port = datagram.senderPort();

        emit clientDiscovered(result);
    }
}
// ------------------------------------------------------------------------------------------------------------------
void CUdpHolePunchingClient::start()
{
    m_Socket = new QUdpSocket(this);

    QByteArray emptyPackage;
    m_Socket->writeDatagram(emptyPackage, host(), port());
}
// ------------------------------------------------------------------------------------------------------------------
