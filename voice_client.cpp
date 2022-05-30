#include "voice_client.h"

#include <QUdpSocket>
// ------------------------------------------------------------------------------------------------------------------
CVoiceClient::CVoiceClient(QObject* parent, uint16_t port)
    : QIODevice(parent), m_Port(port)
{
    m_Socket = new QUdpSocket(this);
    m_Socket->bind(port);
}

qint64 CVoiceClient::readData(char *data, qint64 maxlen)
{

}

qint64 CVoiceClient::writeData(const char *data, qint64 len)
{

}

void CVoiceClient::onSocketReadyRead()
{

}
// ------------------------------------------------------------------------------------------------------------------
