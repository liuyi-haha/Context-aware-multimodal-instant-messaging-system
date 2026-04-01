//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include "sys/common/exception/ApplicationException.h"

namespace sys::core
{
    class ApplicationDomainException : public ApplicationException
    {
    public:
        explicit ApplicationDomainException(const std::string& message) : ApplicationException(message)
        {
        }
    };
}
