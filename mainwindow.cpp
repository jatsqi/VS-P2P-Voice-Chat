#include "mainwindow.h"
#include "connect_remote_widget.h"
#include "metadata_server.h"
#include "metadata_client.h"
// ------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_MainWidget = new CConnectRemoteWidget(this);

    setCentralWidget(m_MainWidget);
}
