#include "channel_widget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QInputDialog>
#include <QListWidget>
// ------------------------------------------------------------------------------------------------------------------
CChannelWidget::CChannelWidget(QWidget* parent, ChannelMetadata metadata)
    : QWidget(parent), m_ChannelMetadata(metadata)
{
    initUi();
}

void CChannelWidget::updateMetadata(const ChannelMetadata& metadata)
{
    m_ChannelMetadata = metadata;
    updateUsersUi();
}

void CChannelWidget::initUi()
{
    m_Layout = new QVBoxLayout(this);
    m_MetadataWidget = new QWidget(this);
    m_MetadataLayout = new QHBoxLayout(m_MetadataWidget);

    m_NameLabel = new QLabel(m_MetadataWidget);
    m_NameLabel->setText(m_ChannelMetadata.channelName);

    m_ConnectButton = new QPushButton("Verbinden", m_MetadataWidget);
    QObject::connect(m_ConnectButton, &QPushButton::pressed, this, &CChannelWidget::onConnectButtonPressed);

    m_MetadataLayout->addWidget(m_NameLabel);
    m_MetadataLayout->addWidget(m_ConnectButton);
    m_MetadataWidget->setLayout(m_MetadataLayout);
    m_Layout->addWidget(m_MetadataWidget);

    m_ClientList = new QListWidget(this);
    updateUsersUi();
    m_Layout->addWidget(m_ClientList);
    //m_Layout->addLayout(m_ClientsLayout);
}

void CChannelWidget::updateUsersUi()
{
    /*QLayoutItem *wItem;
    while ((wItem = m_ClientsLayout->takeAt(0)) != nullptr)
        delete wItem;*/
    m_ClientList->clear();

    for (const UserMetadata &md : m_ChannelMetadata.joinedUsers)
    {
        qDebug() << "Adding user " << md.username << " to layout in channel " << m_ChannelMetadata.channelName;
        m_ClientList->addItem(md.username);
    }
}

void CChannelWidget::onConnectButtonPressed()
{
    bool ok;
    QString password = QInputDialog::getText(this, tr("Passwort erforderlich"),
                                        tr("Passwort:"), QLineEdit::Normal, "", &ok);
   if (ok && !password.isEmpty())
       joinRequested(m_ChannelMetadata.channelName, password);
}
