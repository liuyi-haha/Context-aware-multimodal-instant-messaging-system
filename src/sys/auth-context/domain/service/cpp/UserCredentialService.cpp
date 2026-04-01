//
// Created by 86150 on 2026/3/23.
//

#include "../include/UserCredentialService.h"

#include "sys/common/component/UserCredentialManager.h"
#include "sys/auth-context/domain/object/include/UserCredential.h"
#include "sys/auth-context/domain/exception/InvalidPasswordException.h"
#include "sys/auth-context/domain/exception/UserCredentialServiceDependencyException.h"

namespace sys::auth::domain
{
    UserCredentialService::UserCredentialService(
        port::BackendClient* backendClient,
        QObject* parent)
        : QObject(parent),
          backendClient(backendClient)
    {
    }

    void UserCredentialService::checkConfig() const
    {
        if (backendClient == nullptr)
        {
            throw UserCredentialServiceDependencyException();
        }
    }

    QString UserCredentialService::login(const QString& account, const QString& password)
    {
        checkConfig();

        auto passWord = Password::of(password);

        const auto loginResult = backendClient->login(account, passWord.hashedValue());

        common::component::UserCredentialManager::instance().update(loginResult.userId, loginResult.token);
        return loginResult.userId;
    }
} // namespace sys::auth::domain
