#include "channel_overview_widget.h"
#include "channel_widget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
// ------------------------------------------------------------------------------------------------------------------
CChannelOverviewWidget::CChannelOverviewWidget(QWidget* parent, QString ownName)
    : QWidget(parent), m_OwnUsername(ownName)
{
    initUi();
}

void CChannelOverviewWidget::addChannel(const ChannelMetadata& cm)
{
    m_ChannelsMap.insert(cm.channelName, cm);
    rebuildChannelWidgets();
}

void CChannelOverviewWidget::updateChannel(const ChannelMetadata& cm)
{
    if (!m_ChannelsMap.contains(cm.channelName))
    {
        addChannel(cm);
        return;
    }

    CChannelWidget *widget = m_ChannelsWidgetMap.value(cm.channelName);
    widget->updateMetadata(cm);
}

void CChannelOverviewWidget::removeChannel(const ChannelMetadata& cm)
{

}

void CChannelOverviewWidget::initUi()
{
    m_Layout = new QVBoxLayout(this);
    m_ChannelsLayout = new QVBoxLayout(this);

    m_HeaderLabel = new QLabel(this);
    m_HeaderLabel->setText("P2P Voice Chat Channels");
    m_HeaderLabel->setStyleSheet("font-weight: bold; font-size: 21pt;");
    m_Layout->addWidget(m_HeaderLabel);

    m_HeaderConnectedAs = new QLabel(this);
    m_HeaderConnectedAs->setText("Verbunden als " + m_OwnUsername);
    m_HeaderConnectedAs->setStyleSheet("font-style: italic; font-size: 12pt;");
    m_HeaderConnectedAs->setAlignment(Qt::AlignCenter);
    m_HeaderConnectedAs->setMargin(20);
    m_Layout->addWidget(m_HeaderConnectedAs);

    //m_UpdateButton = new QPushButton(this);
    //m_UpdateButton->setText("Aktualisieren");
    //QObject::connect(m_UpdateButton, &QPushButton::pressed, [this](){ emit updateRequested(); });
    //m_Layout->addWidget(m_UpdateButton);

    rebuildChannelWidgets();

    m_Layout->addLayout(m_ChannelsLayout);
    setLayout(m_Layout);
}

void CChannelOverviewWidget::rebuildChannelWidgets()
{
    m_ChannelsWidgetMap.clear();
    QLayoutItem *wItem;
    while ((wItem = m_ChannelsLayout->takeAt(0)) != nullptr)
        delete wItem;

    for (const ChannelMetadata &metadata : m_ChannelsMap.values())
    {
        CChannelWidget *widget = new CChannelWidget(this, metadata);
        QObject::connect(widget, &CChannelWidget::joinRequested, [this](QString channel, QString password) {
            emit joinRequested(channel, password);
        });

        m_ChannelsLayout->addWidget(widget);
        m_ChannelsWidgetMap.insert(metadata.channelName, widget);
    }
}
