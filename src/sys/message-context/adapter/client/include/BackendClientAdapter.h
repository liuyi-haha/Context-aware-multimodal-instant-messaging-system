//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/message-context/port/client/include/BackendClient.h"

namespace sys::message::adapter
{
    class BackendClientAdapter : public port::BackendClient
    {
    public:
        void sendTextMessage() override;
    };
}
