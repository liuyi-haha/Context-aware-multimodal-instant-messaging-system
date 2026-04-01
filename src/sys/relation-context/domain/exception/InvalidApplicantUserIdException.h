#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class InvalidApplicantUserIdException : public sys::core::DomainException
    {
    public:
        InvalidApplicantUserIdException()
            : DomainException("申请人用户ID不能为空")
        {
        }
    };
}
