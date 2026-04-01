#pragma once

#include <gmock/gmock.h>

#include "sys/user-context/port/repository/include/UserRepository.h"

namespace tests::user::mock
{
    class UserRepositoryMock : public sys::user::port::UserRepository
    {
    public:
        MOCK_METHOD(void, save, (QSharedPointer<sys::user::domain::User> user), (override));
        MOCK_METHOD(QSharedPointer<sys::user::domain::User>, of, (const QString& userId), (override));
        MOCK_METHOD(QList<QSharedPointer<sys::user::domain::User>>, ofAll, (const QSet<QString>&), (override));
    };
}

