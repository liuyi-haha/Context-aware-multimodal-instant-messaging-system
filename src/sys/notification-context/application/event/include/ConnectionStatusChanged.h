#pragma once
#include "sys/notification-context/port/manager/include/ConnectionManager.h"

namespace sys::notification::application
{
    struct ConnectionStatusChanged
    {
        port::ConnectionStatus status = port::ConnectionStatus::Disconnected;
    };
} // namespace sys::notification::application