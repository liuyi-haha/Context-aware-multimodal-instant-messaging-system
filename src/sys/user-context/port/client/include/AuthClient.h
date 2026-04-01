//
// Created by 86150 on 2026/3/23.
//

#pragma once

#include <QString>
#include <QObject>

namespace sys
{
    namespace user::port
    {
        struct AuthResult
        {
            bool isValid;
            QString hashedPassword;
        };

        class AuthClient : public QObject
        {
            Q_OBJECT

        public:
            virtual ~AuthClient() = default;
            virtual AuthResult validateAndHashPassword(const QString& password) = 0;
        };
    }
} // sys

