#include "../include/NotificationContextStarter.h"

#include "dependencypool.h"
#include "sys/notification-context/adapter/publisher/include/ConnectionStatusChangedEventPublisherAdapter.h"
#include "sys/notification-context/adapter/manager/include/NotificationConnectionManagerAdapter.h"
#include "sys/notification-context/application/handler/include/BackendEventHandler.h"
#include "sys/notification-context/application/service/include/NotificationApplicationService.h"
#include "sys/notification-context/remote/subscriber/include/BackendEventSubscriber.h"

namespace sys::notification::starter
{

    void NotificationContextStarter::bootstrap()
    {
        QInjection::addSingleton(new adapter::NotificationConnectionManager());
        QInjection::addSingleton(new adapter::ConnectionStatusChangedEventPublisherAdapter());
        QInjection::addSingleton(new application::NotificationApplicationService);
    }

}
