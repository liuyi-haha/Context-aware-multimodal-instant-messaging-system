//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class UserValidatorDependencyException : public sys::core::DomainException
    {
    public:
        UserValidatorDependencyException()
            : DomainException("用户校验服务依赖未完成配置")
        {
        }
    };
}
