//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>
#include <String>
#include <stdexcept>

namespace sys::core
{
    class ApplicationException : public std::runtime_error
    {
    public:
        explicit ApplicationException(const QString& message) : std::runtime_error(message.toStdString()),
                                                                message_(message)
        {
        }

        const char* what() const noexcept override
        {
            return message_.toLocal8Bit();
        }

    private:
        QString message_;
    };
}
