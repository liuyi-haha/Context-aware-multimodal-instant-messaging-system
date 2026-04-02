//
// Created by 86150 on 2026/3/23.
//

#include "../include/AuthApplicationService.h"

#include <exception>

#include "sys/auth-context/domain/object/include/UserCredential.h"

namespace sys::auth::application
{
    AuthApplicationService::AuthApplicationService(domain::UserCredentialService* userCredentialService,
                                                   QObject* parent)
        : QObject(parent),
          userCredentialService(userCredentialService)
    {
    }

    bool AuthApplicationService::validatePassword(const QString& password)
    {
        if (!domain::Password::checkPassword(password))
        {
            return false;
        }
        return true;
    }

    QString AuthApplicationService::getHashedPassword(const QString& password)
    {
        return password; // Placeholder: hashing algorithm will be implemented later.
    }

    contract::auth::LoginResponse AuthApplicationService::login(const contract::auth::LoginRequest& request)
    {
        contract::auth::LoginResponse response{};
        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }


        try
        {
            response.userId = userCredentialService->login(request.account, request.password);
            response.success = true;
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromUtf8(ex.what());
        }

        return response;
    }

    bool AuthApplicationService::checkConfigAndSetResponse(contract::BaseResponse& response)
    {
        if (userCredentialService == nullptr)
        {
            response.success = false;
            response.errMsg = "AuthApplicationService is not properly configured: userCredentialService is null";
            return false;
        }
        return true;
    }
} // namespace sys::auth::application
