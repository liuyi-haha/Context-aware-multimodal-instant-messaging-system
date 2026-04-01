//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <string>

#include "sys/common/exception/ApplicationException.h"

namespace sys::core
{
    class ApplicationInfraException : public ApplicationException
    {
    public:
        explicit ApplicationInfraException(const std::string& message) : ApplicationException(message)
        {
        }
    };
}
