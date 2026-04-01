//
// Created by 86150 on 2026/3/23.
//
#pragma once

#include <QByteArray>
#include <QSharedPointer>
#include <QString>

#include "dependencyinjector.h"
#include "sys/user-context/port/client/include/AuthClient.h"
#include "sys/user-context/domain/object/include/User.h"

namespace sys
{
    namespace user::domain
    {
        struct ValidationResult
        {
            QSharedPointer<User> user;
            QString hashedPassword;
        };

        class UserValidator : public QObject
        {
            Q_OBJECT

        public:
            explicit UserValidator(port::AuthClient* authClient = QInjection::Inject);
            QString validate(const QString& nickname,
                             const QString& phone,
                             const QString& password,
                             const QByteArray& avatar);

        private:
            port::AuthClient* authClient = nullptr;
            void checkConfig() const;
        };
    }
} // sys
