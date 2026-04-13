//
// Created by 86150 on 2026/3/24.
//

#include "../include/BackendEventHandler.h"

#include <stdexcept>

namespace sys::notification::application
{
    BackendEventHandler::BackendEventHandler(
        port::ConnectionManager* connectionManager,
        port::ConnectionStatusChangedEventPublisher* connectionStatusChangedEventPublisher,
        QObject* parent)
        : QObject(parent),
          connectionManager(connectionManager),
          connectionStatusChangedEventPublisher(connectionStatusChangedEventPublisher)
    {
    }

    void BackendEventHandler::handleConnectedEvent()
    {
        if (connectionManager == nullptr || connectionStatusChangedEventPublisher == nullptr)
        {
            throw std::runtime_error("BackendEventHandler dependencies are not configured.");
        }

        const auto previousStatus = connectionManager->getConnectStatus();
        connectionManager->setConnectionStatus(port::ConnectionStatus::Connected);

        if (previousStatus != connectionManager->getConnectStatus())
        {
            connectionStatusChangedEventPublisher->publish(ConnectionStatusChanged{port::ConnectionStatus::Connected});
        }
    }

    void BackendEventHandler::handleDisconnectedEvent()
    {
        const auto previousStatus = connectionManager->getConnectStatus();
        connectionManager->setConnectionStatus(port::ConnectionStatus::Disconnected);

        if (previousStatus != connectionManager->getConnectStatus())
        {
            connectionStatusChangedEventPublisher->publish(
                ConnectionStatusChanged{port::ConnectionStatus::Disconnected});
        }
    }

    void BackendEventHandler::handleConnectFailedEvent()
    {
        const auto previousStatus = connectionManager->getConnectStatus();
        connectionManager->setConnectionStatus(port::ConnectionStatus::Disconnected);

        if (previousStatus != connectionManager->getConnectStatus())
        {
            connectionStatusChangedEventPublisher->publish(
                ConnectionStatusChanged{port::ConnectionStatus::Disconnected});
        }
    }
}
