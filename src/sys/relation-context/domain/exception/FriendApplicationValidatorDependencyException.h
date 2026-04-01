//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class FriendApplicationValidatorDependencyException : public sys::core::DomainException
    {
    public:
        FriendApplicationValidatorDependencyException()
            : DomainException("好友申请校验器依赖未完成配置")
        {
        }
    };
}
