// ------------------------------------------------------------------------------------------------------------------
#include "port_discovery.h"

#include <QUdpSocket>
#include <QNetworkDatagram>
// ------------------------------------------------------------------------------------------------------------------
void CUdpHolePunchingServer::start()
{
    qDebug() << "Start Hole Punching on Port " << port();
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

        m_Socket->writeDatagram(QByteArray(), datagram.senderAddress(), datagram.senderPort());
        qDebug() << "Discovered " << result.address << " " << result.port;
        m_Socket->close();
        emit clientDiscovered(result);
    }
}
// ------------------------------------------------------------------------------------------------------------------
void CUdpHolePunchingClient::start()
{
    m_Socket = new QUdpSocket(this);
    m_Socket->bind(bindPort());
    //QObject::connect(m_Socket, &QUdpSocket::readyRead, this, &CUdpHolePunchingClient::onSocketReadyRead);

    QByteArray emptyPackage;
    m_Socket->writeDatagram(emptyPackage, host(), destinationPort());
    m_Socket->flush();
    m_Socket->close();
}

void CUdpHolePunchingClient::onSocketReadyRead()
{
    /*if (m_Socket->hasPendingDatagrams())
    {
        qDebug() << "Server responded.";
        emit serverResponded();
    }*/
}
// ------------------------------------------------------------------------------------------------------------------
