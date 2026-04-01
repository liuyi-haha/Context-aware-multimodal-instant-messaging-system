//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class PendingFriendApplicationAlreadyExistsException : public sys::core::DomainException
    {
    public:
        PendingFriendApplicationAlreadyExistsException()
            : DomainException("已存在待处理的好友申请")
        {
        }
    };
}
