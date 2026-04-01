#pragma once
#include "sys/notification-context/port/publisher/include/ConnectionStatusChangedEventPublisher.h"

namespace sys::notification::adapter
{
    class ConnectionStatusChangedEventPublisherAdapter final : public port::ConnectionStatusChangedEventPublisher
    {
    public:
        void publish(const application::ConnectionStatusChanged& event) override;
    };
}