//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::message::domain
{
    class ChatSessionNotFoundException : public core::DomainException
    {
    public:
        explicit ChatSessionNotFoundException()
            : DomainException("聊天会话不存在")
        {
        }
    };
}
