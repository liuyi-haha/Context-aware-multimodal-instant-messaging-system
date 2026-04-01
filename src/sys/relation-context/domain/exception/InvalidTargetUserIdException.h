//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class ApplyFriendToMyselfException : public sys::core::DomainException
    {
    public:
        ApplyFriendToMyselfException()
            : DomainException("不能添加自己为好友")
        {
        }
    };
}
