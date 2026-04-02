#pragma once

#include <gmock/gmock.h>

#include "sys/relation-context/port/client/include/BackendClient.h"

namespace tests::common::mock
{
    class MockBackendClient : public sys::relation::port::BackendClient
    {
    public:
        MOCK_METHOD(
            sys::relation::port::SendFriendApplicationResult,
            sendFriendApplication,
            (const QString& targetUserId, const QString& verificationMessage, const QString& recipientRemark),
            (override));

        MOCK_METHOD(
            sys::relation::port::AcceptFriendApplicationResult,
            acceptFriendApplication,
            (const QString& friendApplicationId, const QString& remark),
            (override));

        MOCK_METHOD(
            void,
            rejectFriendApplication,
            (const QString& friendApplicationId),
            (override));
    };
}

