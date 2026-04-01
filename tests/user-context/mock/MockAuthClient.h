#pragma once

#include <gmock/gmock.h>

#include "sys/user-context/port/client/include/AuthClient.h"

namespace tests::user::mock
{
    class AuthClientMock : public sys::user::port::AuthClient
    {
    public:
        MOCK_METHOD(sys::user::port::AuthResult, validateAndHashPassword, (const QString& password), (override));
    };
}

