#ifndef MAINWINDOW_H
#define MAINWINDOW_H
// ------------------------------------------------------------------------------------------------------------------
#include <QMainWindow>
#include <QAudioFormat>
// ------------------------------------------------------------------------------------------------------------------
class CConnectRemoteWidget;
class CSimpleMetadataServer;
class CLanMetadataClient;
class CChannelOverviewWidget;
class CVoiceClient;
class QAudioSource;
class QAudioSink;
// ------------------------------------------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent);

private slots:
    void onConnectButtonPressed();
    void onClientIdentSuccessful();
    void onClientIdentFailed(QString reason);
    void onClientChannelsUpdated();
    void onClientCurrentChannelUpdated();
    void onClientConnectSuccessful();
    void onClientConnectFailed(QString reason);
    void onJoinChannelRequest(QString channel, QString password);

private:
    void initAudio();

private:
    CConnectRemoteWidget* m_ConnectRemoteWidget;
    CChannelOverviewWidget* m_ChannelOverviewWidget;

    CSimpleMetadataServer* m_MetadataServer;
    CLanMetadataClient* m_MetadataClient;

    QAudioFormat m_AudioFormat;
    CVoiceClient* m_VoiceClient;
    QAudioSink* m_AudioOutput;
    QAudioSource* m_AudioInput;
};
// ------------------------------------------------------------------------------------------------------------------
#endif // MAINWINDOW_H
