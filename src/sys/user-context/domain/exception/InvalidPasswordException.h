//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class InvalidPasswordException : public sys::core::DomainException
    {
    public:
        InvalidPasswordException()
            : DomainException("密码格式不正确")
        {
        }
    };
}
