//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class InvalidAccountException : public sys::core::DomainException
    {
    public:
        InvalidAccountException()
            : DomainException("账号格式不正确，应为手机号或用户ID")
        {
        }
    };
}
