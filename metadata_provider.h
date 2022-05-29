#ifndef METADATA_PROVIDER_H
#define METADATA_PROVIDER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHostAddress>

struct CommunicationMetadata
{
    QString username;
    QHostAddress host;
    uint16_t port;
};

class MetadataProvider : public QObject
{
    Q_OBJECT
public:
    MetadataProvider(QHostAddress provHost, uint16_t provPort);

    virtual void fetchMetadata() = 0;

    QHostAddress providerHost() const;
    uint16_t providerPort() const;

private:
    QHostAddress m_ProviderHost;
    uint16_t m_ProviderPort;

signals:
    void metadataChanged(QList<CommunicationMetadata> metadata);
};

#endif // METADATA_PROVIDER_H
