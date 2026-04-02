#pragma once

#include <gmock/gmock.h>

#include "sys/relation-context/port/repository/include/FriendShipRepository.h"

namespace tests::common::mock
{
    class MockFriendShipRepository : public sys::relation::port::FriendShipRepository
    {
    public:
        MOCK_METHOD(
            QSharedPointer<sys::relation::domain::FriendShip>,
            of,
            (const QString& ownerUserId, const QString& peerUserId),
            (override));

        MOCK_METHOD(
            QSharedPointer<sys::relation::domain::FriendShip>,
            of,
            (const QString& friendShipId),
            (override));


        MOCK_METHOD(
            void,
            save,
            (const QSharedPointer<sys::relation::domain::FriendShip>& friendShip),
            (override));
    };
}

