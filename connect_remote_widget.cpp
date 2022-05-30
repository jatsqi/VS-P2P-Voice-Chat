#include "connect_remote_widget.h"
#include "port_discovery.h"
#include "metadata_server.h"
#include "metadata_client.h"
#include "voice_client.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QCheckBox>

#include <QAudioFormat>
#include <QAudioSource>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioSink>
// ------------------------------------------------------------------------------------------------------------------
CConnectRemoteWidget::CConnectRemoteWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();

    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);
    format = QMediaDevices::defaultAudioOutput().preferredFormat();

    client = new CVoiceClient(this, nullptr, 21213);
    client->open(QIODevice::ReadWrite);

    source = new QAudioSource(QMediaDevices::defaultAudioInput(), format);
    source->start(client);

    sink = new QAudioSink(QMediaDevices::defaultAudioOutput(), format);
    sink->setVolume(1);
    input = sink->start();

    QObject::connect(client, &QIODevice::readyRead, [this]() {
        QByteArray buffer(client->bytesAvailable(), 0);
        qint64 l = client->read(buffer.data(), client->bytesAvailable());
        if (l > 0) {
            input->write(buffer);
        } else {
            qDebug() << "Sorry, l < 0 :(";
        }

        /*static const qint64 BufferSize = 4096;
        const qint64 len = qMin(source->bytesAvailable(), BufferSize);

        QByteArray buffer(len, 0);
        qint64 l = input->read(buffer.data(), len);
        if (l > 0) {
            client->write(buffer);
        } else {
            qDebug() << "Sorry, l < 0 :(";
        }*/
    });

    QObject::connect(sink, &QAudioSink::stateChanged, [](QAudio::State state)
    {
        qDebug() << "State changed to " << state;
    });
}

CConnectRemoteWidget::~CConnectRemoteWidget()
{
    //delete m_Broker;
}

void CConnectRemoteWidget::initForms()
{
    m_LayoutGeneral = new QFormLayout(this);
    m_EditUsername = new QLineEdit();
    m_LayoutGeneral->addRow("Benutzername", m_EditUsername);

    m_LayoutMetadata = new QFormLayout(this);
    m_EditIp = new QLineEdit(this);
    m_EditIp->setInputMask("000.000.000.000;_");
    m_LayoutMetadata->addRow("IP Adresse des Metadaten-Servers: ", m_EditIp);

    m_EditPort = new QLineEdit(this);
    m_EditPort->setInputMask("00000;_");
    m_LayoutMetadata->addRow("Port des Metadaten-Servers: ", m_EditPort);

    m_CheckBoxLocalServer = new QCheckBox(this);
    m_CheckBoxLocalServer->setChecked(false);
    m_LayoutMetadata->addRow("Metadata-Server lokale Starten", m_CheckBoxLocalServer);

    QObject::connect(m_CheckBoxLocalServer, &QCheckBox::clicked, this, &CConnectRemoteWidget::onCheckBoxLocalServerPressed);

    m_MainLayout->addLayout(m_LayoutGeneral);
    m_MainLayout->addLayout(m_LayoutMetadata);
}

void CConnectRemoteWidget::initUi()
{
    m_MainLayout = new QVBoxLayout(this);
    m_MainLayout->setAlignment(Qt::AlignCenter);

    m_LabelHeader = new QLabel(this);
    m_LabelHeader->setTextFormat(Qt::TextFormat::RichText);
    m_LabelHeader->setText("P2P Voice Chat Connector");
    m_LabelHeader->setStyleSheet("font-weight: bold; color: black; font-size: 21pt;");
    m_MainLayout->addWidget(m_LabelHeader);

    initForms();

    m_ButtonConnect = new QPushButton(this);
    m_ButtonConnect->setText("Verbinden");
    QObject::connect(m_ButtonConnect, &QPushButton::pressed, this, &CConnectRemoteWidget::onConnectButtonPressed);
    m_MainLayout->addWidget(m_ButtonConnect);

    setLayout(m_MainLayout);
}

void CConnectRemoteWidget::onConnectButtonPressed()
{

}

void CConnectRemoteWidget::onCheckBoxLocalServerPressed(bool state)
{
    m_EditIp->setEnabled(!state);
    m_EditPort->setEnabled(!state);
}
// ------------------------------------------------------------------------------------------------------------------