//
// Created by 86150 on 2026/3/28.
//

#pragma once
#include <exception>

#include <QByteArray>
#include <QString>

namespace sys::core
{
    class InfraException : public std::exception
    {
    public:
        explicit InfraException(const QString& message)
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
