#include "../include/UserBackendClientAdapter.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QTemporaryFile>
#include "OAIUserDefaultApi.h"
#include "OAIUserHttpRequest.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"
#include "sys/user-context/domain/exception/InvalidAccountException.h"
#include "sys/user-context/domain/exception/PhoneHasBeenRegistered.h"
#include "sys/user-context/domain/exception/InvalidNicknameException.h"
#include "sys/user-context/domain/exception/UserNotFoundException.h"


namespace sys::user::adapter
{
    namespace
    {
        constexpr const char* kErrCodeNicknameInvalid = "NICKNAME_INVALID";
        constexpr const char* kErrCodePhoneRegistered = "PHONE_REGISTERED";
        constexpr const char* kErrCodeUnknownError = "UNKNOWN_ERROR";
        constexpr const char* kSearchErrCodeKeywordInvalid = "KEYWORD_INVALID";
        constexpr const char* kSearchErrCodeUserNotFound = "USER_NOT_FOUND";
    }

    OpenAPIUser::OAIUserRegisterUser_200_response UserServerApiGateway::registerUser(
        const QString& nickname, const QString& phone, const OpenAPIUser::OAIUserHttpFileElement& avatar,
        const QString& password)
    {
        OpenAPIUser::OAIUserRegisterUser_200_response response;
        OpenAPIUser::OAIUserDefaultApi api;
        QNetworkAccessManager manager;
        api.setNetworkAccessManager(&manager);
        QString networkErrorString;

        QEventLoop loop;
        api.setTimeOut(8000);
        QObject::connect(&api,
                         &OpenAPIUser::OAIUserDefaultApi::registerUserSignal,
                         &loop,
                         [&](const OpenAPIUser::OAIUserRegisterUser_200_response& summary)
                         {
                             response = summary;
                             loop.quit();
                         });

        QObject::connect(&api,
                         &OpenAPIUser::OAIUserDefaultApi::registerUserSignalError,
                         &loop,
                         [&](const OpenAPIUser::OAIUserRegisterUser_200_response& summary,
                             QNetworkReply::NetworkError,
                             const QString& error)
                         {
                             networkErrorString = error;
                             loop.quit();
                         });

        api.registerUser(nickname, avatar, phone, password);

        loop.exec();

        if (!response.is_data_Set())
        {
            throw core::InfraException(networkErrorString);
        }

        return response;
    }

    OpenAPIUser::OAIUserSearchUser_200_response UserServerApiGateway::searchUser(const QString& keyword)
    {
        OpenAPIUser::OAIUserSearchUser_200_response response;

        OpenAPIUser::OAIUserDefaultApi api;
        api.setBearerToken(sys::common::component::UserCredentialManager::instance().getCurrentToken());
        QNetworkAccessManager manager;
        api.setNetworkAccessManager(&manager);
        api.setTimeOut(8000);
        api.setBearerToken(common::component::UserCredentialManager::instance().getCurrentToken());
        QString networkErrorString;

        QEventLoop loop;
        QObject::connect(&api,
                         &OpenAPIUser::OAIUserDefaultApi::searchUserSignal,
                         &loop,
                         [&](const OpenAPIUser::OAIUserSearchUser_200_response& summary)
                         {
                             response = summary;
                             loop.quit();
                         });

        QObject::connect(&api,
                         &OpenAPIUser::OAIUserDefaultApi::searchUserSignalError,
                         &loop,
                         [&](const OpenAPIUser::OAIUserSearchUser_200_response&,
                             QNetworkReply::NetworkError,
                             const QString& error)
                         {
                             networkErrorString = error;
                             loop.quit();
                         });

        api.searchUser(keyword);
        loop.exec();

        if (!networkErrorString.isEmpty())
        {
            throw core::InfraException(networkErrorString);
        }

        return response;
    }

    BackendClientAdapter::RegisterUserErrCode BackendClientAdapter::fromErrCode(const QString& errCode)
    {
        if (errCode.isEmpty())
        {
            return RegisterUserErrCode::None;
        }

        if (errCode == QString::fromLatin1(kErrCodeNicknameInvalid))
        {
            return RegisterUserErrCode::NicknameInvalid;
        }

        if (errCode == QString::fromLatin1(kErrCodePhoneRegistered))
        {
            return RegisterUserErrCode::PhoneRegistered;
        }

        if (errCode == QString::fromLatin1(kErrCodeUnknownError))
        {
            return RegisterUserErrCode::UnknownError;
        }

        return RegisterUserErrCode::UnknownError;
    }

    QString BackendClientAdapter::toErrCode(const RegisterUserErrCode errCode)
    {
        switch (errCode)
        {
        case RegisterUserErrCode::None:
            return QString();
        case RegisterUserErrCode::NicknameInvalid:
            return QString::fromLatin1(kErrCodeNicknameInvalid);
        case RegisterUserErrCode::PhoneRegistered:
            return QString::fromLatin1(kErrCodePhoneRegistered);
        case RegisterUserErrCode::UnknownError:
        default:
            return QString::fromLatin1(kErrCodeUnknownError);
        }
    }

    BackendClientAdapter::SearchUserErrCode BackendClientAdapter::fromSearchErrCode(const QString& errCode)
    {
        if (errCode.isEmpty())
        {
            return SearchUserErrCode::None;
        }

        if (errCode == QString::fromLatin1(kSearchErrCodeUserNotFound))
        {
            return SearchUserErrCode::UserNotFound;
        }

        if (errCode == QString::fromLatin1(kSearchErrCodeKeywordInvalid))
        {
            return SearchUserErrCode::KeywordInvalid;
        }

        return SearchUserErrCode::UnknownError;
    }

    port::BackendClient::RegisterUserResult BackendClientAdapter::registerUser(
        const QString& hashedPassword, const QString& nickname, const QString& phone, const QFileInfo& avatarFileInfo)
    {
        if (apiGateway == nullptr)
        {
            throw core::InfraException("UserServiceApiGateway 未配置");
        }

        auto fileElement = OpenAPIUser::OAIUserHttpFileElement();

        fileElement.setVariableName("avatar");
        fileElement.setFileName(avatarFileInfo.absoluteFilePath()); //很关键
        fileElement.setMimeType("image/png");
        fileElement.setRequestFileName(avatarFileInfo.fileName());
        const auto response = apiGateway->registerUser(nickname, phone, fileElement, hashedPassword);
        if (!response.isSuccess())
        {
            const RegisterUserErrCode errCode = fromErrCode(response.getErrCode());
            switch (errCode)
            {
            case RegisterUserErrCode::PhoneRegistered:
                throw domain::PhoneHasBeenRegisteredException();
            case RegisterUserErrCode::NicknameInvalid:
                throw domain::InvalidNicknameException();
            case RegisterUserErrCode::UnknownError:
                throw core::InfraException(response.getErrMsg());
            case RegisterUserErrCode::None:
            default:
                throw core::InfraException(response.getErrMsg());
            }
        }

        const auto data = response.getData();
        return {data.getUserId(), data.getAvatarFileId()};
    }

    QSharedPointer<domain::User> BackendClientAdapter::searchUser(const QString& account)
    {
        if (apiGateway == nullptr)
        {
            throw core::InfraException("UserServiceApiGateway 未配置");
        }

        const auto response = apiGateway->searchUser(account);
        if (!response.isSuccess())
        {
            switch (fromSearchErrCode(response.getErrCode()))
            {
            case SearchUserErrCode::UserNotFound:
                throw domain::UserNotFoundException();
            case SearchUserErrCode::KeywordInvalid:
                throw domain::InvalidAccountException();
            case SearchUserErrCode::UnknownError:
            case SearchUserErrCode::None:
            default:
                throw core::InfraException(response.getErrMsg());
            }
        }

        if (!response.is_data_Set())
        {
            return nullptr;
        }

        const auto data = response.getData();
        return domain::User::of(
            data.getUserId(),
            data.getNickname(),
            data.getPhone(),
            QByteArray(),
            data.getAvatarFileId(),
            data.getDescription());
    }
}
