#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class InvalidApplyTimeException : public sys::core::DomainException
    {
    public:
        InvalidApplyTimeException()
            : DomainException("申请时间不合法")
        {
        }
    };
}
