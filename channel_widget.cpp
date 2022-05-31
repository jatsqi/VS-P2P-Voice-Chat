#include "channel_widget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QInputDialog>
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
    m_MetadataLayout = new QHBoxLayout(this);
    m_ClientsLayout = new QVBoxLayout(this);
    m_ClientsLayout->setContentsMargins(QMargins(10, 10, 10, 10));

    m_NameLabel = new QLabel(this);
    m_NameLabel->setText(m_ChannelMetadata.channelName);
    m_ConnectButton = new QPushButton("Verbinden", this);
    QObject::connect(m_ConnectButton, &QPushButton::pressed, this, &CChannelWidget::onConnectButtonPressed);
    m_MetadataLayout->addWidget(m_NameLabel);
    m_MetadataLayout->addWidget(m_ConnectButton);

    m_Layout->addLayout(m_MetadataLayout);

    updateUsersUi();
    m_Layout->addLayout(m_ClientsLayout);

    setLayout(m_Layout);
}

void CChannelWidget::updateUsersUi()
{
    QLayoutItem *wItem;
    while ((wItem = m_ClientsLayout->takeAt(0)) != nullptr)
        delete wItem;

    for (const UserMetadata &md : m_ChannelMetadata.joinedUsers)
    {
        qDebug() << "Adding user " << md.username << " to layout!";
        QLabel *user = new QLabel(this);
        user->setPixmap(style()->standardPixmap(QStyle::SP_ComputerIcon));
        user->setText(md.username);

        m_ClientsLayout->addWidget(user);
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
