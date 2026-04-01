//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class FriendShipAlreadyExistsException : public sys::core::DomainException
    {
    public:
        FriendShipAlreadyExistsException()
            : DomainException("双方已经是好友")
        {
        }
    };
}
