//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include "sys/common/exception/DomainException.h"


namespace sys::core
{
    class DomainValidationException : DomainException
    {
    public:
        explicit DomainValidationException(const std::string& message) : DomainException(message)
        {
        }
    };
}
