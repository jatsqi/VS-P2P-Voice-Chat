#include "voice_client.h"
#include "metadata_client.h"

#include <QUdpSocket>
#include <QDataStream>
#include <QNetworkDatagram>
// ------------------------------------------------------------------------------------------------------------------
CVoiceClient::CVoiceClient(QObject* parent, CLanMetadataClient *metadataClient, QAudioFormat format)
    : QIODevice(parent), m_MetadataClient(metadataClient), m_Format(format)
{
    m_Socket = new QUdpSocket(this);
    QObject::connect(m_Socket, &QUdpSocket::readyRead, this, &CVoiceClient::onSocketReadyRead);
    m_Socket->bind(port());
}

qint64 CVoiceClient::readData(char *data, qint64 maxlen)
{
    quint64 len = qMin(maxlen, m_ReadBuffer.size());
    memcpy(data, m_ReadBuffer.data(), len);
    m_ReadBuffer = QByteArray(m_ReadBuffer.data() + len, m_ReadBuffer.size() - len);

    qDebug() << "Reading " << len << " bytes of data :)" << " maxlen: " << maxlen << " buffer: " << m_ReadBuffer.size();
    return len;
}

qint64 CVoiceClient::writeData(const char *data, qint64 len)
{
    QByteArray output;
    QDataStream stream(&output, QIODevice::WriteOnly);

    const ChannelMetadata *currentChannel = m_MetadataClient->currentChannel();
    if (currentChannel == nullptr)
        return 0;

    stream << m_MetadataClient->username();
    stream << QByteArray(data, len);

    // Loop über alle User im aktuellen Channel (aktuell maximal ein Client)
    for(auto &a : currentChannel->joinedUsers)
    {
        if (a.username == m_MetadataClient->username())
           continue;

        // Dieser Fall tritt auf, wenn ein Client und der Server auf dem selben Host liegen
        // Der Server broadcastet an alle anderen Clients die IP 127.0.0.1, was aus seiner Sicht
        // der Client auf dem Host ist.
        // Stattdessen wird die IP des Metadaten-Servers genutzt.
        QHostAddress destination = a.host;
        if (destination.isLoopback())
            destination = m_MetadataClient->host();

        // Sende Daten an anderen Client
        m_Socket->writeDatagram(output, a.host, a.port);
    }

    return len;
}

uint16_t CVoiceClient::port() const
{
    return m_MetadataClient->prefferedVoicePort();
}

void CVoiceClient::onSocketReadyRead()
{
    // Solange eingehende Datagramme erkannt
    while (m_Socket->hasPendingDatagrams())
    {
        // Datagramm lesen
        QNetworkDatagram dg = m_Socket->receiveDatagram();
        QByteArray data = dg.data();
        QDataStream stream(data);

        // Von wem kommen die Daten => vorbereitung für mehr als ein weiterer Client im Channel
        QString username;
        stream >> username;

        // PCM Daten speichern
        QByteArray voiceData;
        stream >> voiceData;

        m_ReadBuffer.append(voiceData);
        qDebug() << "Received voice from " << username << " with size " << voiceData.size() << " rdbuffer: " << m_ReadBuffer.size();

        emit readyRead();

        // Dieses Signal ist aktuell ungenutzt
        if (m_ReadBuffer.size() > m_Format.bytesForDuration(1000000)) {
            emit voiceDataReady();
        }
    }
}
// ------------------------------------------------------------------------------------------------------------------
