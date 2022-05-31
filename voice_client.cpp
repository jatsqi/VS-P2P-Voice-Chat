#include "voice_client.h"
#include "metadata_client.h"

#include <QUdpSocket>
#include <QDataStream>
#include <QNetworkDatagram>
// ------------------------------------------------------------------------------------------------------------------
CVoiceClient::CVoiceClient(QObject* parent, CLanMetadataClient *metadataClient, QAudioFormat format)
    : QIODevice(parent), m_MetadataClient(metadataClient), m_Format(format)
{
    qDebug() << "Pre Bind";
    m_Socket = new QUdpSocket(this);
    QObject::connect(m_Socket, &QUdpSocket::readyRead, this, &CVoiceClient::onSocketReadyRead);
    bool b = m_Socket->bind(port());
    qDebug() << "Bind? " << b;
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

    for(auto &a : currentChannel->joinedUsers)
    {
        if (a.username == m_MetadataClient->username())
           continue;
        m_Socket->writeDatagram(output, a.host, a.port);

        qDebug() << "Sending voice data to " << a.username << " (Host: " << a.host << ", Port: " << a.port << ")";
        qDebug() << "I listen on port " << port();
    }

    return len;
}

uint16_t CVoiceClient::port() const
{
    return m_MetadataClient->prefferedVoicePort();
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

    if (m_ReadBuffer.size() > m_Format.bytesForDuration(1000000)) {
        emit readyRead();
        emit voiceDataReady();
    }
}
// ------------------------------------------------------------------------------------------------------------------
