#pragma once
#include <QObject>

#include "dependencyinjector.h"
#include "sys/notification-context/application/event/include/ConnectionStatusChanged.h"
#include "sys/notification-context/port/manager/include/ConnectionManager.h"
#include "sys/notification-context/port/publisher/include/ConnectionStatusChangedEventPublisher.h"

namespace sys::notification::application
{
    class NotificationApplicationService : public QObject
    {
        Q_OBJECT

    public:
        NotificationApplicationService(
            port::ConnectionManager* connectionManager = QInjection::Inject,
            port::ConnectionStatusChangedEventPublisher* connectionStatusChangedEventPublisher = QInjection::Inject,
            QObject* parent = nullptr);
        void tryToConnect();
        int getFriendApplicationColumnCount();
        int getMessageColumnCount();
        int getChatSessionItemCount(const QString& chatSessionId);

    private:
        port::ConnectionManager* connectionManager = nullptr;
        port::ConnectionStatusChangedEventPublisher* connectionStatusChangedEventPublisher = nullptr;
    };
} // namespace sys::notification::application
