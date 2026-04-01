#pragma once

#include <gmock/gmock.h>

#include "sys/relation-context/port/repository/include/PrivateChatSessionRepository.h"

namespace tests::relation::mock
{
    class MockPrivateChatSessionRepository : public sys::relation::port::PrivateChatSessionRepository
    {
    public:
        MOCK_METHOD(void, save, (const QSharedPointer<sys::relation::domain::PrivateChatSession>& privateChatSession),
                    (override));
        MOCK_METHOD(QSharedPointer<sys::relation::domain::PrivateChatSession>, of,
                    (const QString& privateChatSessionId), (override));
        MOCK_METHOD(QSharedPointer<sys::relation::domain::PrivateChatSession>, ofFriendShipId,
                    (const QString& friendShipId), (override));
        MOCK_METHOD(QList<QSharedPointer<sys::relation::domain::PrivateChatSession>>, ofAll,
                    (const QSet<QString>& privateChatSessionIds), (override));
    };
}
