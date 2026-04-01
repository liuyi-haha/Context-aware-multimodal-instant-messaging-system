#include "../include/ConnectionStatusChangedEventPublisherAdapter.h"

#include "common/EventBus.h"

namespace sys::notification::adapter
{
    void ConnectionStatusChangedEventPublisherAdapter::publish(const application::ConnectionStatusChanged& event)
    {
        emit common::EventBus::instance()->connectionStatusChanged(event);
    }
}
