#include "mainwindow.h"
#include "connect_remote_widget.h"
#include "channel_overview_widget.h"
#include "metadata_client.h"
#include "metadata_server.h"
#include "voice_client.h"

#include <QMessageBox>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioSource>
// ------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_VoiceClient(nullptr)
{
    m_ConnectRemoteWidget = new CConnectRemoteWidget(this);
    QObject::connect(m_ConnectRemoteWidget, &CConnectRemoteWidget::connectButtonPressed, this, &MainWindow::onConnectButtonPressed);
    setCentralWidget(m_ConnectRemoteWidget);
}

void MainWindow::onConnectButtonPressed()
{
    uint16_t serverPort = m_ConnectRemoteWidget->inputPort();
    QHostAddress serverAddress;

    if (m_ConnectRemoteWidget->inputStartLocal())
    {
        serverAddress = QHostAddress::LocalHost;
        m_MetadataServer = new CSimpleMetadataServer(this, serverPort);
        m_MetadataServer->createChannel("Plaudern 1", "passwort");
        m_MetadataServer->createChannel("Plaudern 2", "passwort");
    }
    else
    {
        serverAddress = m_ConnectRemoteWidget->inputHost();
    }

    m_MetadataClient = new CLanMetadataClient(this, m_ConnectRemoteWidget->inputUsername(), serverAddress, serverPort);
    QObject::connect(m_MetadataClient, &CLanMetadataClient::identificationSuccessful, this, &MainWindow::onClientIdentSuccessful);
    QObject::connect(m_MetadataClient, &CLanMetadataClient::identificationFailed, this, &MainWindow::onClientIdentFailed);
    QObject::connect(m_MetadataClient, &CLanMetadataClient::connectionSuccessful, this, &MainWindow::onClientConnectSuccessful);
    QObject::connect(m_MetadataClient, &CLanMetadataClient::connectionFailed, this, &MainWindow::onClientConnectFailed);
    QObject::connect(m_MetadataClient, &CLanMetadataClient::channelsUpdated, this, &MainWindow::onClientChannelsUpdated);
    QObject::connect(m_MetadataClient, &CLanMetadataClient::currentChannelUpdated, this, &MainWindow::onClientCurrentChannelUpdated);
    m_MetadataClient->connect();
}

void MainWindow::onClientIdentSuccessful()
{
    m_ChannelOverviewWidget = new CChannelOverviewWidget(this, m_MetadataClient->username());
    QObject::connect(m_ChannelOverviewWidget, &CChannelOverviewWidget::joinRequested, this, &MainWindow::onJoinChannelRequest);
    setCentralWidget(m_ChannelOverviewWidget);
}

void MainWindow::onClientIdentFailed(QString reason)
{
    QMessageBox::critical(this, "Fehler bei der Identifizierung.", reason);
}

void MainWindow::onClientChannelsUpdated()
{
    for (const auto &a : m_MetadataClient->channels())
    {
        qDebug() << "Adding Channel " << a.channelName;
        m_ChannelOverviewWidget->updateChannel(a);
    }
}

void MainWindow::onClientCurrentChannelUpdated()
{
    if (m_MetadataClient->currentChannel() == nullptr)
        return;

    m_ChannelOverviewWidget->updateChannel(*m_MetadataClient->currentChannel());
}

void MainWindow::onClientConnectSuccessful()
{
    initAudio();
}

void MainWindow::onClientConnectFailed(QString reason)
{
    QMessageBox::critical(this, "Fehler beim Beitreten", reason);
}

void MainWindow::onJoinChannelRequest(QString channel, QString password)
{
    qDebug() << "Joining channel " << channel << " with " << password;
    m_MetadataClient->joinChannel(channel, password);
}

void MainWindow::initAudio()
{
    m_AudioFormat.setChannelCount(1);
    m_AudioFormat.setSampleRate(4000);
    m_AudioFormat.setSampleFormat(QAudioFormat::Int16);

    if (m_VoiceClient != nullptr)
    {
        m_AudioOutput->stop();
        m_AudioInput->stop();

        delete m_AudioInput;
        delete m_AudioOutput;
        delete m_VoiceClient;
    }

    m_VoiceClient = new CVoiceClient(this, m_MetadataClient, m_AudioFormat);
    m_VoiceClient->open(QIODevice::ReadWrite);

    m_AudioOutput = new QAudioSink(QMediaDevices::defaultAudioOutput(), m_AudioFormat, this);
    m_AudioOutput->setVolume(30);

    m_AudioInput = new QAudioSource(QMediaDevices::defaultAudioInput(), m_AudioFormat, this);
    m_AudioInput->start(m_VoiceClient);
    QObject::connect(m_VoiceClient, &CVoiceClient::voiceDataReady, [this]() {
        if (m_AudioOutput->state() != QAudio::ActiveState)
        {
            m_AudioOutput->start(m_VoiceClient);
        }
    });
}
