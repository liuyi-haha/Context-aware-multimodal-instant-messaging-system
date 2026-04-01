//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class InvalidNicknameException : public sys::core::DomainException
    {
    public:
        InvalidNicknameException()
            : DomainException("昵称不符合规范")
        {
        }
    };
}
