//
// Created by 86150 on 2026/3/27.
//

#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class FriendApplicationHandleUnauthorizedException : public core::DomainException
    {
    public:
        FriendApplicationHandleUnauthorizedException()
            : DomainException("无权处理该好友申请")
        {
        }
    };
}

