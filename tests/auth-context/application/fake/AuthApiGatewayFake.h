#pragma once

#include <optional>
#include <QVector>

#include "sys/auth-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/auth-context/domain/object/include/UserCredential.h"

namespace tests::auth::fake
{
    class AuthApiGatewayFake : public sys::auth::adapter::IAuthServiceApiGateway
    {
    public:
        struct FakeUser
        {
            QString account;
            QString password;
            QString userId;
            QString token;
        };

    public:
        OpenAPIAuth::OAIAuthLogin_200_response login(const OpenAPIAuth::OAIAuthLogin_request& request) override
        {
            ++loginCallCount;
            lastRequest = request;

            for (const auto& user : fakeUsers)
            {
                const QString hashedPassword = sys::auth::domain::Password::of(user.password).hashedValue();
                if (user.account == request.getAccount() && hashedPassword == request.getPassword())
                {
                    return successResponse(user.userId, user.token);
                }
            }

            return failureResponse("ACCOUNT_PASSWORD_NOT_MATCH", "账号或密码错误");
        }

        const FakeUser& defaultUser() const
        {
            return fakeUsers.front();
        }

        static OpenAPIAuth::OAIAuthLogin_200_response successResponse(const QString& userId, const QString& token)
        {
            OpenAPIAuth::OAIAuthLogin_200_response response;
            OpenAPIAuth::OAIAuthLogin_200_response_data data;
            data.setUserId(userId);
            data.setToken(token);

            response.setSuccess(true);
            response.setData(data);
            response.setErrCode(QString());
            response.setErrMsg(QString());
            return response;
        }

        static OpenAPIAuth::OAIAuthLogin_200_response failureResponse(const QString& errCode, const QString& errMsg)
        {
            OpenAPIAuth::OAIAuthLogin_200_response response;
            response.setSuccess(false);
            response.setErrCode(errCode);
            response.setErrMsg(errMsg);
            return response;
        }

    public:
        int loginCallCount = 0;
        std::optional<OpenAPIAuth::OAIAuthLogin_request> lastRequest;

    private:
        const QVector<FakeUser> fakeUsers = {
            {"13800138000", "abc_123", "123456789", "token-001"},
            {"13900139000", "hello_456", "234567891", "token-002"}
        };
    };
}
