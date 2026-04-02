#pragma once

#include <gmock/gmock.h>

#include "sys/common/port/UserClient.h"

namespace tests::common::mock
{
    class UserClientMock : public sys::common::port::UserClient
    {
    public:
        MOCK_METHOD(sys::common::port::UserInfo, getUser, (const QString& userId), (override));
        MOCK_METHOD((QHash<QString, sys::common::port::UserInfo>),
                    getUsers,
                    (const QSet<QString>& userIds),
                    (override));
    };
}

