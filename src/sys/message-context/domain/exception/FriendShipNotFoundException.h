//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::message::domain
{
    class FriendShipNotFoundException : public core::DomainException
    {
    public:
        explicit FriendShipNotFoundException()
            : DomainException("你们已经不是好友了")
        {
        }
    };
}
