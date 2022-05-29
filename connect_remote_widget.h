#ifndef CONNECTREMOTE_H
#define CONNECTREMOTE_H
// ------------------------------------------------------------------------------------------------------------------
#include <QMainWindow>
// ------------------------------------------------------------------------------------------------------------------
class QVBoxLayout;
class QLabel;
class QPushButton;
class QLineEdit;
class QFormLayout;
// ------------------------------------------------------------------------------------------------------------------
class LocalMetadataBroker;
// ------------------------------------------------------------------------------------------------------------------
class CConnectRemoteWidget : public QWidget
{
    Q_OBJECT

public:
    CConnectRemoteWidget(QWidget *parent = nullptr);
    ~CConnectRemoteWidget();

private:
    void initForms();
    void initUi();

private:
    QVBoxLayout *m_MainLayout;

    QLabel *m_LabelHeader;
    QPushButton* m_ButtonConnect;

    QFormLayout *m_LayoutForm;
    QWidget *m_WidgetForm;
    QLineEdit* m_EditName;
    QLineEdit* m_EditIp;

    LocalMetadataBroker *m_Broker;

public slots:
    void onConnectButtonPressed();
};
// ------------------------------------------------------------------------------------------------------------------
#endif // CONNECTREMOTE_H
