//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/message-context/port/client/include/BackendClient.h"
#include <gmock/gmock.h>
#include <QString>

namespace tests::message::mock
{
    class BackendClientMock : public sys::message::port::BackendClient
    {
    public:
        MOCK_METHOD(void, sendTextMessage, (const QString& chatSessionId, const QString& text), (override));
    };
}
