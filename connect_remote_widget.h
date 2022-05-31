#ifndef CONNECTREMOTE_H
#define CONNECTREMOTE_H
// ------------------------------------------------------------------------------------------------------------------
#include <QMainWindow>
#include <QHostAddress>
// ------------------------------------------------------------------------------------------------------------------
class QVBoxLayout;
class QLabel;
class QPushButton;
class QLineEdit;
class QFormLayout;
class QCheckBox;

class CVoiceClient;
class QAudioSource;
class QIODevice;
class QAudioSink;
class QAudioOutput;
// ------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------
class CConnectRemoteWidget : public QWidget
{
    Q_OBJECT

public:
    CConnectRemoteWidget(QWidget *parent = nullptr);
    ~CConnectRemoteWidget();

    QHostAddress inputHost() const;
    QString inputUsername() const;
    uint16_t inputPort() const;
    bool inputStartLocal() const;

signals:
    void connectButtonPressed();

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

    QAudioOutput *output;
    QAudioSink *sink;
    QAudioSource *source;
    QIODevice *input;
    CVoiceClient *client;

public slots:
    void onConnectButtonPressed();
    void onCheckBoxLocalServerPressed(bool state);
};
// ------------------------------------------------------------------------------------------------------------------
#endif // CONNECTREMOTE_H
