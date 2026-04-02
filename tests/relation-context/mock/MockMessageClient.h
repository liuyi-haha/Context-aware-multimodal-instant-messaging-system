#pragma once

#include <gmock/gmock.h>
#include "sys/relation-context/port/client/include/MessageClient.h"

namespace tests::common::mock
{
    class MockMessageClient : public sys::relation::port::MessageClient
    {
    public:
        MOCK_METHOD((QHash<QString, LastMessageInfo>), getLastMessageInfos, (const QList<QString>& chatSessionIds),
                    (override));
    };
}
