//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class InvalidUserIdException : public sys::core::DomainException
    {
    public:
        InvalidUserIdException()
            : DomainException("用户ID不合法")
        {
        }
    };
}
