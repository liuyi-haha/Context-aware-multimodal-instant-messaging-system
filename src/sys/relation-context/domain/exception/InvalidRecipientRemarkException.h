//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class InvalidRecipientRemarkException : public sys::core::DomainException
    {
    public:
        InvalidRecipientRemarkException()
            : DomainException("对方备注长度不合法")
        {
        }
    };
}
