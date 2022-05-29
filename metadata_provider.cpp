#include "metadata_provider.h"

MetadataProvider::MetadataProvider(QHostAddress provHost, uint16_t provPort)
    : m_ProviderHost(provHost), m_ProviderPort(provPort)
{
}

QHostAddress MetadataProvider::providerHost() const
{
    return this->m_ProviderHost;
}

uint16_t MetadataProvider::providerPort() const
{
    return this->m_ProviderPort;
}
