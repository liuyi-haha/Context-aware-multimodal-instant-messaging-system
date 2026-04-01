//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class InvalidVerificationMessageException : public sys::core::DomainException
    {
    public:
        InvalidVerificationMessageException()
            : DomainException("验证消息长度不合法")
        {
        }
    };
}
