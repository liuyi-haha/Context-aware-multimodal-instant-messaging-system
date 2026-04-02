//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::message::domain
{
    class NotParticipantException : public core::DomainException
    {
    public:
        NotParticipantException()
            : DomainException("你不在该聊天会话中")
        {
        }
    };
}
