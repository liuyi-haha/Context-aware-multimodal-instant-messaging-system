#pragma once

#include <gmock/gmock.h>

#include "sys/relation-context/port/repository/include/ParticipantRepository.h"

namespace tests::common::mock
{
    class MockParticipantRepository : public sys::relation::port::ParticipantRepository
    {
    public:
        MOCK_METHOD(void, saveAll,
                    (const QVector<QSharedPointer<sys::relation::domain::Participant>>& participants), (override)
        );
        MOCK_METHOD(QSharedPointer<sys::relation::domain::Participant>, of, (const QString& participantId), (override)
        );
        MOCK_METHOD(QSharedPointer<sys::relation::domain::Participant>, of, (const QString& privateChatSessionId,
                        const QString& userId), (override)
        );
        MOCK_METHOD(QList<QSharedPointer<sys::relation::domain::Participant>>, ofAll, (const QString& userId,
                        sys::relation::domain::ParticipantRole role), (override)
        );
        MOCK_METHOD(QList<QSharedPointer<sys::relation::domain::Participant>>, ofChatSessionId,
                    (const QString& chatSesionId), (override)
        );
    };
}
