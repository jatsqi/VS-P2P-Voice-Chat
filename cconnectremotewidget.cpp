#include "cconnectremotewidget.h"
#include "ui_cconnectremotewidget.h"

CConnectRemoteWidget::CConnectRemoteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CConnectRemoteWidget)
{
    ui->setupUi(this);
}

CConnectRemoteWidget::~CConnectRemoteWidget()
{
    delete ui;
}
