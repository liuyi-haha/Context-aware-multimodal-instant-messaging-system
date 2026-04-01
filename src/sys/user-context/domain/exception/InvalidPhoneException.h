//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class InvalidPhoneException : public sys::core::DomainException
    {
    public:
        InvalidPhoneException()
            : DomainException("手机号格式不正确")
        {
        }
    };
}
