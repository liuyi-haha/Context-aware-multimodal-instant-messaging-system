#pragma once

#include <gmock/gmock.h>

#include "sys/user-context/port/client/include/FileClient.h"

namespace tests::user::mock
{
    class FileClientMock : public sys::user::port::FileClient
    {
    public:
        MOCK_METHOD(void, uploadAvatar, (const QSharedPointer<sys::user::domain::User>& user), (override));
    };
}

