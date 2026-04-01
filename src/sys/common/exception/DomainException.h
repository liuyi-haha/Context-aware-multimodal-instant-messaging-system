//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <exception>
#include <QByteArray>
#include <QString>

namespace sys::core
{
    class DomainException : public std::exception
    {
    public:
        explicit DomainException(const QString& message)
            : messageUtf8_(message.toUtf8())
        {
        }

        const char* what() const noexcept override
        {
            return messageUtf8_.constData();
        }

    private:
        QByteArray messageUtf8_;
    };
}
