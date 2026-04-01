#pragma once

#include <vector>

#include "sys/notification-context/port/publisher/include/ConnectionStatusChangedEventPublisher.h"

namespace tests::notification::fake
{
    class ConnectionStatusChangedEventPublisherFake final
        : public sys::notification::port::ConnectionStatusChangedEventPublisher
    {
    public:
        void publish(const sys::notification::application::ConnectionStatusChanged& event) override
        {
            ++publishCount;
            events.push_back(event);
        }

    public:
        int publishCount = 0;
        std::vector<sys::notification::application::ConnectionStatusChanged> events;
    };
}

