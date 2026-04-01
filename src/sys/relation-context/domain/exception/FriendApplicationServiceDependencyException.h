//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    class FriendApplicationServiceDependencyException : public sys::core::DomainException
    {
    public:
        FriendApplicationServiceDependencyException()
            : DomainException("好友申请领域服务依赖未完成配置")
        {
        }
    };
}
