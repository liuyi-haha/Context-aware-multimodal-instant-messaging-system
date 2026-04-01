#pragma once

#include <gmock/gmock.h>

#include "sys/relation-context/port/client/include/NotificationClient.h"

namespace tests::relation::mock
{
    class MockNotificationClient : public sys::relation::port::NotificationClient
    {
    public:
        MOCK_METHOD(
            int,
            getUnreadChatSessionItemCount,
            (const QString& chatSessionId),
            (override));
    };
}
