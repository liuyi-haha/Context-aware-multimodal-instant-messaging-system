//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class FriendApplicationAlreadyProcessedException : public core::DomainException
    {
    public:
        FriendApplicationAlreadyProcessedException()
            : DomainException("好友申请已被处理")
        {
        }
    };
}
