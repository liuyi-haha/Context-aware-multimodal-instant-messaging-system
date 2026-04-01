#pragma once
#include "sys/notification-context/application/event/include/ConnectionStatusChanged.h"
#include <QObject>
namespace sys::notification::port
{
    class ConnectionStatusChangedEventPublisher : public QObject
    {
        Q_OBJECT
    public:
        virtual ~ConnectionStatusChangedEventPublisher() = default;
        virtual void publish(const application::ConnectionStatusChanged& event) = 0;
    };
} // namespace sys::notification::port