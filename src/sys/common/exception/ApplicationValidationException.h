//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include "sys/common/exception/ApplicationException.h"

namespace sys::core
{
    class ApplicationValidationException : public ApplicationException
    {
    public:
        explicit ApplicationValidationException(const std::string& message) : ApplicationException(message)
        {
        }
    };
}
