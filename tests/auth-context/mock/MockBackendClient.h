#pragma once

#include <gmock/gmock.h>

#include "sys/auth-context/port/client/include/BackendClient.h"

namespace tests::auth::mock
{
    class BackendClientMock : public sys::auth::port::BackendClient
    {
    public:
        MOCK_METHOD(LoginResult,
                    login,
                    (const QString& account, const QString& hashedPassword),
                    (override));
    };
}
