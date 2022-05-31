#ifndef CHANNEL_WIDGET_H
#define CHANNEL_WIDGET_H
// ------------------------------------------------------------------------------------------------------------------
#include <QWidget>

#include "metadata.h"
// ------------------------------------------------------------------------------------------------------------------
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
// ------------------------------------------------------------------------------------------------------------------
class CChannelWidget : public QWidget
{
    Q_OBJECT
public:
    CChannelWidget(QWidget* parent, ChannelMetadata metadata);

    void updateMetadata(const ChannelMetadata& metadata);
    ChannelMetadata channelMetadata() const { return m_ChannelMetadata; }

private:
    void initUi();
    void updateUsersUi();

signals:
    void joinRequested(QString channelName, QString password);

private slots:
    void onConnectButtonPressed();

private:
    ChannelMetadata m_ChannelMetadata;

    QLabel* m_NameLabel;
    QPushButton* m_ConnectButton;
    QVBoxLayout* m_Layout;
    QHBoxLayout* m_MetadataLayout;
    QVBoxLayout* m_ClientsLayout;
};

#endif // CHANNEL_WIDGET_H
