//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/message-context/port/client/include/RelationClient.h"

namespace sys::message::adapter
{
    class RelationClientAdapter : public port::RelationClient
    {
    public:
        void checkSenderHasPermissionToSendMessage() override;
    };
}
