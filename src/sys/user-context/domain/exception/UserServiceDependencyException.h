//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::user::domain
{
    class UserServiceDependencyException : public sys::core::DomainException
    {
    public:
        UserServiceDependencyException()
            : DomainException("用户领域服务依赖未完成配置")
        {
        }
    };
}
