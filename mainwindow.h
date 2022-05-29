#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent);

private:
    QWidget* m_MainWidget;
};

#endif // MAINWINDOW_H
