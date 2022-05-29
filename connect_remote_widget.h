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
class QCheckBox;
// ------------------------------------------------------------------------------------------------------------------

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

    QFormLayout* m_LayoutGeneral;
    QLineEdit* m_EditUsername;

    QFormLayout *m_LayoutMetadata;
    QLineEdit* m_EditIp;
    QLineEdit* m_EditPort;
    QCheckBox* m_CheckBoxLocalServer;

    QPushButton* m_ButtonConnect;

public slots:
    void onConnectButtonPressed();
    void onCheckBoxLocalServerPressed(bool state);
};
// ------------------------------------------------------------------------------------------------------------------
#endif // CONNECTREMOTE_H
