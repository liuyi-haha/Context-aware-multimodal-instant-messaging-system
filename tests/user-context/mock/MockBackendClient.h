#pragma once

#include <gmock/gmock.h>

#include "sys/user-context/port/client/include/BackendClient.h"

namespace tests::user::mock
{
    class BackendClientMock : public sys::user::port::BackendClient
    {
    public:
        MOCK_METHOD(RegisterUserResult,
                    registerUser,
                    (const QString& hashedPassword, const QString& nickname, const QString& phone, const QFileInfo&
                        avatar),
                    (override));

        MOCK_METHOD(QSharedPointer<sys::user::domain::User>,
                    searchUser,
                    (const QString& account),
                    (override));
    };
}

