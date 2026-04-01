//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class PhoneHasBeenRegisteredException : public sys::core::DomainException
    {
    public:
        explicit PhoneHasBeenRegisteredException()
            : DomainException("手机号已被注册")
        {
        }
    };
}
