//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::auth::domain
{
    class AccountPasswordNotMatchedException : public sys::core::DomainException
    {
    public:
        AccountPasswordNotMatchedException()
            : DomainException("账号或密码错误")
        {
        }
    };
}
