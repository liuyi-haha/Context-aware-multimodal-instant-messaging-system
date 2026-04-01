#pragma once

#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class InvalidAvatarException : public sys::core::DomainException
    {
    public:
        InvalidAvatarException()
            : DomainException("头像不能为空")
        {
        }
    };
}

