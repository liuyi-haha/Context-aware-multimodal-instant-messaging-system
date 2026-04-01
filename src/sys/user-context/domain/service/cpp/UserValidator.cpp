//
// Created by 86150 on 2026/3/23.
//

#include "../include/UserValidator.h"

#include "sys/user-context/domain/exception/InvalidAvatarException.h"
#include "sys/user-context/domain/exception/InvalidNicknameException.h"
#include "sys/user-context/domain/exception/InvalidPasswordException.h"
#include "sys/user-context/domain/exception/InvalidPhoneException.h"
#include "sys/user-context/domain/exception/UserValidatorDependencyException.h"

namespace sys
{
    namespace user
    {
        namespace domain
        {
            UserValidator::UserValidator(port::AuthClient* authClient)
                : authClient(authClient)
            {
            }

            void UserValidator::checkConfig() const
            {
                if (authClient == nullptr)
                {
                    throw UserValidatorDependencyException();
                }
            }

            QString UserValidator::validate(
                const QString& nickname,
                const QString& phone,
                const QString& password,
                const QByteArray& avatar)
            {
                checkConfig();

                if (avatar.isEmpty())
                {
                    throw InvalidAvatarException();
                }

                if (!Nickname::checkNickname(nickname))
                {
                    throw InvalidNicknameException();
                }
                if (!Phone::checkPhone(phone))
                {
                    throw InvalidPhoneException();
                }
                const auto authResult = authClient->validateAndHashPassword(password);
                if (!authResult.isValid)
                {
                    throw InvalidPasswordException();
                }

                return authResult.hashedPassword;
            }
        } // domain
    } // user
} // sys
