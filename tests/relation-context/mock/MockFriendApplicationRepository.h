#pragma once

#include <gmock/gmock.h>

#include "sys/relation-context/port/repository/include/FriendApplicationRepository.h"

namespace tests::common::mock
{
    class MockFriendApplicationRepository : public sys::relation::port::FriendApplicationRepository
    {
    public:
        using FriendApplicationPtr = QSharedPointer<sys::relation::domain::FriendApplication>;
        using FriendApplicationList = QList<FriendApplicationPtr>;

        MOCK_METHOD(
            FriendApplicationPtr,
            of,
            (const QString& applicantUserId, const QString& targetUserId, sys::relation::domain::ApplicationStatus
                status),
            (override));

        MOCK_METHOD(
            FriendApplicationPtr,
            of,
            (const QString& friendApplicationId),
            (override));

        MOCK_METHOD(
            FriendApplicationList,
            ofApplicantUserId,
            (const QString& applicantUserId),
            (override));

        MOCK_METHOD(
            FriendApplicationList,
            ofTargetUserId,
            (const QString& ofTargetUserId),
            (override));

        MOCK_METHOD(
            void,
            save,
            (FriendApplicationPtr friendApplication),
            (override));
    };
}

