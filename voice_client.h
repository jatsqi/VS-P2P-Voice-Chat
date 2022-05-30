#ifndef VOICECLIENT_HPP
#define VOICECLIENT_HPP
// ------------------------------------------------------------------------------------------------------------------
#include <QObject>
#include <QIODevice>
// ------------------------------------------------------------------------------------------------------------------
class QUdpSocket;
// ------------------------------------------------------------------------------------------------------------------
class CVoiceClient : public QIODevice
{
    Q_OBJECT
public:
    CVoiceClient(QObject *parent, uint16_t port);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    uint16_t port() const { return m_Port; }

public slots:
    void onSocketReadyRead();

signals:
    void voiceDataReady();

private:
    uint16_t m_Port;
    QUdpSocket *m_Socket;
    QByteArray m_ReadBuffer;
};
// ------------------------------------------------------------------------------------------------------------------
#endif
// ------------------------------------------------------------------------------------------------------------------
