#include "mainwindow.h"
#include "connect_remote_widget.h"
// ------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_MainWidget = new CConnectRemoteWidget(this);

    setCentralWidget(m_MainWidget);
}
