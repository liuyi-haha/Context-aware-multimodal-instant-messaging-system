//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::message::domain
{
    class InvalidTextException : public sys::core::DomainException
    {
    public:
        explicit InvalidTextException()
            : DomainException("消息文本长度不合法") // 为空或者太长
        {
        }
    };
}
