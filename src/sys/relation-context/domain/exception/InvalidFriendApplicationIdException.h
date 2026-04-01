#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class InvalidFriendApplicationIdException : public sys::core::DomainException
    {
    public:
        InvalidFriendApplicationIdException()
            : DomainException("好友申请ID不能为空")
        {
        }
    };
}
