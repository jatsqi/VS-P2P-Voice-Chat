#include "connect_remote_widget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
// ------------------------------------------------------------------------------------------------------------------
CConnectRemoteWidget::CConnectRemoteWidget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
}

CConnectRemoteWidget::~CConnectRemoteWidget()
{
    //delete m_Broker;
}

void CConnectRemoteWidget::initForms()
{
    m_WidgetForm = new QWidget(this);
    m_LayoutForm = new QFormLayout(m_WidgetForm);

    m_EditName = new QLineEdit(m_WidgetForm);
    m_LayoutForm->addRow("Benutzername: ", m_EditName);

    m_EditIp = new QLineEdit(m_WidgetForm);
    m_EditIp->setInputMask("000.000.000.000;_");
    m_LayoutForm->addRow("IP Adresse des Ziels: ", m_EditIp);

    m_WidgetForm->setLayout(m_LayoutForm);
    m_MainLayout->addWidget(m_WidgetForm);
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
// ------------------------------------------------------------------------------------------------------------------
