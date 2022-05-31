#ifndef CHANNEL_OVERVIEW_WIDGET_H
#define CHANNEL_OVERVIEW_WIDGET_H
// ------------------------------------------------------------------------------------------------------------------
#include <QWidget>

#include "metadata.h"
// ------------------------------------------------------------------------------------------------------------------
class QVBoxLayout;
class QLabel;
class CChannelWidget;
class QPushButton;
// ------------------------------------------------------------------------------------------------------------------
class CChannelOverviewWidget : public QWidget
{
    Q_OBJECT
public:
    CChannelOverviewWidget(QWidget* parent, QString ownName);

    void addChannel(const ChannelMetadata& cm);
    void updateChannel(const ChannelMetadata& cm);
    void removeChannel(const ChannelMetadata& cm);

private:
    void initUi();
    void rebuildChannelWidgets();

signals:
    void updateRequested();
    void joinRequested(QString channel, QString password);

private:
    QMap<QString, ChannelMetadata> m_ChannelsMap;
    QMap<QString, CChannelWidget*> m_ChannelsWidgetMap;

    QString m_OwnUsername;

    QLabel* m_HeaderLabel;
    QLabel* m_HeaderConnectedAs;
    QPushButton* m_UpdateButton;
    QVBoxLayout* m_ChannelsLayout;
    QVBoxLayout* m_Layout;
};
// ------------------------------------------------------------------------------------------------------------------
#endif // CHANNEL_OVERVIEW_WIDGET_H
// ------------------------------------------------------------------------------------------------------------------
