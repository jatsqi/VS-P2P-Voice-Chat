#include "voice_client.h"
#include "metadata_client.h"

#include <QUdpSocket>
#include <QDataStream>
#include <QNetworkDatagram>
// ------------------------------------------------------------------------------------------------------------------
CVoiceClient::CVoiceClient(QObject* parent, CLanMetadataClient *metadataClient, uint16_t port)
    : QIODevice(parent), m_MetadataClient(metadataClient), m_Port(port)
{
    m_Socket = new QUdpSocket(this);

    QObject::connect(m_Socket, &QUdpSocket::readyRead, this, &CVoiceClient::onSocketReadyRead);

    m_Socket->bind(port);
}

qint64 CVoiceClient::readData(char *data, qint64 maxlen)
{
    quint64 len = qMin(maxlen, m_ReadBuffer.size());
    memcpy(data, m_ReadBuffer.data(), len);

    qDebug() << "Reading " << len << " bytes of data :)" << " maxlen: " << maxlen << " buffer: " << m_ReadBuffer.size();
    return len;
}

qint64 CVoiceClient::writeData(const char *data, qint64 len)
{
    QByteArray output;
    QDataStream stream(&output, QIODevice::WriteOnly);

    QString name = "TerraStormDE";
    stream << name;
    //stream << m_MetadataClient->username();
    qDebug() << "Sending DATAGRAM with size " << len;
    stream << QByteArray(data, len);

    //for(auto &a : m_MetadataClient->usersInCurrentChannel())
    //{
    m_Socket->writeDatagram(output, QHostAddress::LocalHost, m_Port);
    //}

    return len;
}

void CVoiceClient::onSocketReadyRead()
{
    while (m_Socket->hasPendingDatagrams())
    {
        QNetworkDatagram dg = m_Socket->receiveDatagram();
        QByteArray data = dg.data();
        QDataStream stream(data);

        QString username;
        stream >> username;

        QByteArray voiceData;
        stream >> voiceData;

        m_ReadBuffer.append(voiceData);
        qDebug() << "Received voice from " << username << " with size " << voiceData.size() << " rdbuffer: " << m_ReadBuffer.size();
    }

    if (m_ReadBuffer.size() > 50000) {
        emit readyRead();
        m_ReadBuffer.clear();
    }
}
// ------------------------------------------------------------------------------------------------------------------
