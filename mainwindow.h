#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QVBoxLayout;
class QLabel;
class QPushButton;
class QLineEdit;
class QFormLayout;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initForms();
    void initUi();

private:
    QLabel *m_LabelHeader;

    QWidget* m_MainWidget;
    QVBoxLayout *m_MainLayout;

    QPushButton* m_ButtonConnect;

    QFormLayout *m_LayoutForm;
    QWidget *m_WidgetForm;
    QLineEdit* m_EditName;
    QLineEdit* m_EditIp;

public slots:
    void onConnectButtonPressed();
};
#endif // MAINWINDOW_H
