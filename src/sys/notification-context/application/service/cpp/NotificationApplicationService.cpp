#include "../include/NotificationApplicationService.h"

#include <stdexcept>

#include "dependencyinjector.h"

namespace sys::notification::application
{
    NotificationApplicationService::NotificationApplicationService(
        port::ConnectionManager* connectionManager,
        port::ConnectionStatusChangedEventPublisher* connectionStatusChangedEventPublisher,
        QObject* parent)
        : QObject(parent),
          connectionManager(connectionManager),
          connectionStatusChangedEventPublisher(connectionStatusChangedEventPublisher)
    {
    }

    void NotificationApplicationService::tryToConnect()
    {
        if (connectionManager == nullptr)
        {
            throw std::runtime_error("ConnectionManager is not configured.");
        }

        port::ConnectionStatus regionStatus = connectionManager->getConnectStatus();
        connectionManager->tryToConnect();
        if (regionStatus != connectionManager->getConnectStatus())
        {
            connectionStatusChangedEventPublisher->publish(ConnectionStatusChanged{
                connectionManager->getConnectStatus()
            });
        }
    }

    int NotificationApplicationService::getFriendApplicationColumnCount()
    {
        return 0;
    }

    int NotificationApplicationService::getMessageColumnCount()
    {
        return 0;
    }

    int NotificationApplicationService::getChatSessionItemCount(const QString& chatSessionId)
    {
        return 0;
    }
} // namespace sys::notification::application
