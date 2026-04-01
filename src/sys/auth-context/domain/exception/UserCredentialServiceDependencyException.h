//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::auth::domain
{
    class UserCredentialServiceDependencyException : public sys::core::DomainException
    {
    public:
        UserCredentialServiceDependencyException()
            : DomainException("凭据领域服务依赖未完成配置")
        {
        }
    };
}
