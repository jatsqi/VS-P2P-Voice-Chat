#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initForms()
{
    m_WidgetForm = new QWidget(m_MainWidget);
    m_LayoutForm = new QFormLayout(m_WidgetForm);

    m_EditName = new QLineEdit(m_WidgetForm);
    m_LayoutForm->addRow("Benutzername: ", m_EditName);

    m_EditIp = new QLineEdit(m_WidgetForm);
    m_EditIp->setInputMask("000.000.000.000;_");
    m_LayoutForm->addRow("IP Adresse des Ziels: ", m_EditIp);

    m_WidgetForm->setLayout(m_LayoutForm);
    m_MainLayout->addWidget(m_WidgetForm);
}

void MainWindow::initUi()
{
    m_MainWidget = new QWidget(this);
    m_MainLayout = new QVBoxLayout(m_MainWidget);
    m_MainLayout->setAlignment(Qt::AlignCenter);

    m_LabelHeader = new QLabel(m_MainWidget);
    m_LabelHeader->setTextFormat(Qt::TextFormat::RichText);
    m_LabelHeader->setText("P2P Voice Chat Connector");
    m_LabelHeader->setStyleSheet("font-weight: bold; color: black; font-size: 21pt;");
    m_MainLayout->addWidget(m_LabelHeader);

    initForms();

    m_ButtonConnect = new QPushButton(m_MainWidget);
    m_ButtonConnect->setText("Verbinden");
    QObject::connect(m_ButtonConnect, &QPushButton::pressed, this, &MainWindow::onConnectButtonPressed);
    m_MainLayout->addWidget(m_ButtonConnect);

    m_MainWidget->setLayout(m_MainLayout);
    setCentralWidget(m_MainWidget);
}

void MainWindow::onConnectButtonPressed()
{

}
