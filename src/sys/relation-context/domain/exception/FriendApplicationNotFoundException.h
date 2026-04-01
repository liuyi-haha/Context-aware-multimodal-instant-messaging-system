//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class FriendApplicationNotFoundException : public core::DomainException
    {
    public:
        FriendApplicationNotFoundException()
            : DomainException("好友申请不存在")
        {
        }
    };
}
