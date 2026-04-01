//
// Created by 86150 on 2026/3/24.
//
#pragma once
#include <QObject>

#include "dependencyinjector.h"
#include "sys/notification-context/port/manager/include/ConnectionManager.h"
#include "sys/notification-context/port/publisher/include/ConnectionStatusChangedEventPublisher.h"

namespace sys::notification::application
{
    class BackendEventHandler : public QObject
    {
        Q_OBJECT

    public:
        explicit BackendEventHandler(
            port::ConnectionManager* connectionManager = QInjection::Inject,
            port::ConnectionStatusChangedEventPublisher* connectionStatusChangedEventPublisher = QInjection::Inject,
            QObject* parent = nullptr);

        void handleConnectedEvent();

    private:
        port::ConnectionManager* connectionManager = nullptr;
        port::ConnectionStatusChangedEventPublisher* connectionStatusChangedEventPublisher = nullptr;
    };
}
