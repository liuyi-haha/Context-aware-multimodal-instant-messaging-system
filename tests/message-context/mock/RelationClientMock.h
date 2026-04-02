//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <gmock/gmock.h>

#include "sys/message-context/port/client/RelationClient.h"

namespace tests::message::mock
{
    class RelationClientMock : public sys::message::port::RelationClient
    {
    public:
        MOCK_METHOD(void, checkSenderHasPermissionToSendMessage, (const QString& chatSessionId), (override)
        );
    };
}
