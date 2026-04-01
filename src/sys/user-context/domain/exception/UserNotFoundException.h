//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class UserNotFoundException : public sys::core::DomainException
    {
    public:
        explicit UserNotFoundException()
            : DomainException("用户不存在")
        {
        }
    };
}
