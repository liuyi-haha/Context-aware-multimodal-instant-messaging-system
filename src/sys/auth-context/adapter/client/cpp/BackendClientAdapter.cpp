#include "../include/BackendClientAdapter.h"

#include <QEventLoop>
#include <QNetworkAccessManager>

#include "OAIAuthDefaultApi.h"
#include "sys/auth-context/domain/exception/AccountPasswordNotMatchedException.h"
#include "sys/common/exception/InfraExcception.h"

namespace sys::auth::adapter
{
    namespace
    {
        constexpr const char* kErrCodeAccountPasswordNotMatch = "ACCOUNT_PASSWORD_NOT_MATCH";
        constexpr const char* kErrCodeUnknownError = "UNKNOWN_ERROR";
    }

    OpenAPIAuth::OAIAuthLogin_200_response AuthServerApiGateway::login(const OpenAPIAuth::OAIAuthLogin_request& request)
    {
        OpenAPIAuth::OAIAuthDefaultApi api;
        QNetworkAccessManager manager;
        api.setNetworkAccessManager(&manager);
        api.setTimeOut(8000);

        OpenAPIAuth::OAIAuthLogin_200_response response;
        QString networkError;

        QEventLoop loop;
        QObject::connect(&api,
                         &OpenAPIAuth::OAIAuthDefaultApi::loginSignal,
                         &loop,
                         [&](const OpenAPIAuth::OAIAuthLogin_200_response& summary)
                         {
                             response = summary;
                             loop.quit();
                         });

        QObject::connect(&api,
                         &OpenAPIAuth::OAIAuthDefaultApi::loginSignalError,
                         &loop,
                         [&](const OpenAPIAuth::OAIAuthLogin_200_response&, QNetworkReply::NetworkError,
                             const QString& error)
                         {
                             networkError = error;
                             loop.quit();
                         });

        api.login(request);
        loop.exec();

        if (!networkError.isEmpty())
        {
            throw core::InfraException(networkError);
        }

        return response;
    }

    BackendClientAdapter::BackendClientAdapter(IAuthServiceApiGateway* apiGateway)
    {
        if (apiGateway == nullptr)
        {
            ownedApiGateway = std::make_unique<AuthServerApiGateway>();
            this->apiGateway = ownedApiGateway.get();
            return;
        }

        this->apiGateway = apiGateway;
    }

    BackendClientAdapter::LoginErrCode BackendClientAdapter::fromErrCode(const QString& errCode)
    {
        if (errCode.isEmpty())
        {
            return LoginErrCode::None;
        }

        if (errCode == QString::fromLatin1(kErrCodeAccountPasswordNotMatch))
        {
            return LoginErrCode::AccountPasswordNotMatch;
        }

        if (errCode == QString::fromLatin1(kErrCodeUnknownError))
        {
            return LoginErrCode::UnknownError;
        }

        return LoginErrCode::UnknownError;
    }

    port::BackendClient::LoginResult BackendClientAdapter::login(const QString& account, const QString& hashedPassword)
    {
        if (apiGateway == nullptr)
        {
            throw core::InfraException("AuthServiceApiGateway 未配置");
        }

        OpenAPIAuth::OAIAuthLogin_request request;
        request.setAccount(account);
        request.setPassword(hashedPassword);

        const auto response = apiGateway->login(request);

        if (!response.isSuccess())
        {
            const auto errCode = fromErrCode(response.getErrCode());
            if (errCode == LoginErrCode::AccountPasswordNotMatch)
            {
                throw domain::AccountPasswordNotMatchedException();
            }

            throw core::InfraException(response.getErrMsg());
        }

        if (!response.is_data_Set())
        {
            throw core::InfraException("登录响应缺少 data");
        }

        const auto data = response.getData();
        if (!data.is_user_id_Set() || !data.is_token_Set())
        {
            throw core::InfraException("登录响应缺少 userId 或 token");
        }

        return {data.getUserId(), data.getToken()};
    }
}
