#pragma once

#include <gmock/gmock.h>

#include "sys/relation-context/port/client/include/UserClient.h"

namespace tests::relation::mock
{
    class UserClientMock : public sys::relation::port::UserClient
    {
    public:
        MOCK_METHOD(sys::relation::port::UserInfo, getUser, (const QString& userId), (override));
        MOCK_METHOD((QHash<QString, sys::relation::port::UserInfo>),
                    getUsers,
                    (const QSet<QString>& userIds),
                    (override));
    };
}

