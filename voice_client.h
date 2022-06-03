#ifndef VOICECLIENT_HPP
#define VOICECLIENT_HPP
// ------------------------------------------------------------------------------------------------------------------
#include <QObject>
#include <QIODevice>
#include <QAudioFormat>
// ------------------------------------------------------------------------------------------------------------------
class QUdpSocket;
class CLanMetadataClient;
// ------------------------------------------------------------------------------------------------------------------
class CVoiceClient : public QIODevice
{
    Q_OBJECT
public:
    CVoiceClient(QObject *parent, CLanMetadataClient *metadataClient, QAudioFormat format);

    // Liest Daten aus dem Puffer (Daten kommen von anderen Gesprächsteilnehmern)
    qint64 readData(char *data, qint64 maxlen) override;

    // Schreibt Daten in UDP Socket -> zu anderen Clients
    qint64 writeData(const char *data, qint64 len) override;

    // Wie viele Daten sind aktuell im Puffer?
    qint64 bytesAvailable() const override { return m_ReadBuffer.size() + QIODevice::bytesAvailable(); }
    
    // Aktuell gleich zu bytesAvailable()
    qint64 size() const override { return m_ReadBuffer.size(); }

    // Port, auf dem Voice-Pakete empfangen werden
    uint16_t port() const;

public slots:
    void onSocketReadyRead();

signals:
    void voiceDataReady();

private:
    CLanMetadataClient *m_MetadataClient;
    QAudioFormat m_Format;
    QUdpSocket *m_Socket;
    QByteArray m_ReadBuffer;
};
// ------------------------------------------------------------------------------------------------------------------
#endif
// ------------------------------------------------------------------------------------------------------------------
