#pragma once

#include "sys/notification-context/adapter/manager/include/NotificationConnectionManagerAdapter.h"

namespace tests::notification::fake
{
    class WebsocketGatewayFake final : public sys::notification::adapter::IWebsocketGateway
    {
    public:
        void open() override
        {
            ++openCallCount;
        }

    public:
        int openCallCount = 0;
    };
}

