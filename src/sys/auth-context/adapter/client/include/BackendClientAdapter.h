#pragma once

#include <memory>

#include "OAIAuthLogin_200_response.h"
#include "OAIAuthLogin_request.h"
#include "sys/auth-context/port/client/include/BackendClient.h"

namespace sys::auth::adapter
{
    class IAuthServiceApiGateway
    {
    public:
        virtual ~IAuthServiceApiGateway() = default;
        virtual OpenAPIAuth::OAIAuthLogin_200_response login(const OpenAPIAuth::OAIAuthLogin_request& request) = 0;
    };

    class AuthServerApiGateway final : public IAuthServiceApiGateway
    {
    public:
        OpenAPIAuth::OAIAuthLogin_200_response login(const OpenAPIAuth::OAIAuthLogin_request& request) override;
    };

    class BackendClientAdapter final : public port::BackendClient
    {
    public:
        enum class LoginErrCode
        {
            None,
            AccountPasswordNotMatch,
            UnknownError
        };

    public:
        explicit BackendClientAdapter(IAuthServiceApiGateway* apiGateway = nullptr);

        LoginResult login(const QString& account, const QString& hashedPassword) override;

    private:
        static LoginErrCode fromErrCode(const QString& errCode);

    private:
        std::unique_ptr<IAuthServiceApiGateway> ownedApiGateway;
        IAuthServiceApiGateway* apiGateway = nullptr;
    };
}
