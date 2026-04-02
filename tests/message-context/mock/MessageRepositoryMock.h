//
// Created by 86150 on 2026/4/2.
//

#pragma once
#include "gmock/gmock.h"
#include "sys/message-context/port/repository/MessageRepository.h"

namespace tests::message::mock
{
    class MessageRepositoryMock : public sys::message::port::MessageRepository
    {
    public:
        MOCK_METHOD(void, save, (const QSharedPointer<sys::message::domain::Message>), (override));
        MOCK_METHOD(QSharedPointer<sys::message::domain::Message>, of, (const QString& messageId), (override));
        MOCK_METHOD(void, saveAll, (const QList<QSharedPointer<sys::message::domain::Message>>&), (override));
        MOCK_METHOD(QList<QSharedPointer<sys::message::domain::Message>>, ofRecentMessages,
                    (const QString& chatSessionId, int count), (override));
    };
}
