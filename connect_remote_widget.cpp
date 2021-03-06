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
#include <QMessageBox>
// ------------------------------------------------------------------------------------------------------------------
CConnectRemoteWidget::CConnectRemoteWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
    m_EditUsername->setFocus();

    /*m_EditUsername->setText("hello5");
    m_EditPort->setText("12345");
    m_EditIp->setText("127.000.000.001");
    m_CheckBoxLocalServer->setChecked(true);*/
}

CConnectRemoteWidget::~CConnectRemoteWidget()
{
    //delete m_Broker;
}

QHostAddress CConnectRemoteWidget::inputHost() const
{
    return QHostAddress(m_EditIp->text());
}

QString CConnectRemoteWidget::inputUsername() const
{
    return m_EditUsername->text();
}

uint16_t CConnectRemoteWidget::inputPort() const
{
    return m_EditPort->text().toUInt();
}

bool CConnectRemoteWidget::inputStartLocal() const
{
    return m_CheckBoxLocalServer->isChecked();
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
    if (m_EditUsername->text().size() < 3)
    {
        QMessageBox::critical(this, "Eingaben Fehlerhaft", "Der Nutzername muss mehr als 3 Zeichen beinhalten.");
        return;
    }

    if (m_EditIp->text().size() != 15 && !m_CheckBoxLocalServer->isChecked())
    {
        QMessageBox::critical(this, "Eingaben Fehlerhaft", "Die IPv4 muss aus genau 12 Zahlen bestehen.");
        return;
    }

    if (m_EditPort->text().isEmpty())
    {
        QMessageBox::critical(this, "Eingaben Fehlerhaft", "Der Port muss aus mindestens einer Zahl bestehen.");
        return;
    }

    emit connectButtonPressed();
}

void CConnectRemoteWidget::onCheckBoxLocalServerPressed(bool state)
{
    m_EditIp->setEnabled(!state);
}
// ------------------------------------------------------------------------------------------------------------------
