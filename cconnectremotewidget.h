#ifndef CCONNECTREMOTEWIDGET_H
#define CCONNECTREMOTEWIDGET_H

#include <QWidget>

namespace Ui {
class CConnectRemoteWidget;
}

class CConnectRemoteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CConnectRemoteWidget(QWidget *parent = nullptr);
    ~CConnectRemoteWidget();

private:
    Ui::CConnectRemoteWidget *ui;
};

#endif // CCONNECTREMOTEWIDGET_H
