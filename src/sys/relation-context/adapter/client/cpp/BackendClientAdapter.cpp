#include "../include/BackendClientAdapter.h"

#include <QDateTime>
#include <QEventLoop>
#include <QNetworkAccessManager>

#include "OAIChatDefaultApi.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"
#include "sys/relation-context/domain/exception/FriendApplicationAlreadyProcessedException.h"
#include "sys/relation-context/domain/exception/FriendApplicationNotFoundException.h"
#include "sys/relation-context/domain/exception/FriendRelationAlreadyExistsException.h"
#include "sys/relation-context/domain/exception/InvalidTargetUserIdException.h"
#include "sys/relation-context/domain/exception/PendingFriendApplicationAlreadyExistsException.h"

namespace sys::relation::adapter
{
    namespace
    {
        constexpr const char* kErrCodeUserNotFound = "USER_NOT_FOUND";
        constexpr const char* kErrCodeDuplicateApplication = "DUPLICATE_APPLICATION";
        constexpr const char* kErrCodeAlreadyFriend = "ALREADY_FRIEND";
        constexpr const char* kErrCodeSelfFriend = "SELF_FRIEND";
        constexpr const char* kErrCodeUnknownError = "UNKNOWN_ERROR";
        constexpr const char* kErrCodeApplicationNotFound = "APPLICATION_NOT_FOUND";
        constexpr const char* kErrCodeApplicationAlreadyHandled = "APPLICATION_ALREADY_HANDLED";
    }

    OpenAPIChat::OAIChatSendFriendApplication_200_response ChatServerApiGateway::sendFriendApplication(
        const OpenAPIChat::OAIChatSendFriendApplication_request& request)
    {
        OpenAPIChat::OAIChatSendFriendApplication_200_response response;

        OpenAPIChat::OAIChatDefaultApi api;
        // 设置 token
        api.setBearerToken(sys::common::component::UserCredentialManager::instance().getCurrentToken());
        QNetworkAccessManager manager;
        api.setNetworkAccessManager(&manager);
        api.setTimeOut(8000);
        QString networkError;

        QEventLoop loop;
        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::sendFriendApplicationSignal,
                         &loop,
                         [&](const OpenAPIChat::OAIChatSendFriendApplication_200_response& summary)
                         {
                             response = summary;
                             loop.quit();
                         });

        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::sendFriendApplicationSignalError,
                         &loop,
                         [&](const OpenAPIChat::OAIChatSendFriendApplication_200_response&,
                             QNetworkReply::NetworkError,
                             const QString& error)
                         {
                             networkError = error;
                             loop.quit();
                         });

        api.sendFriendApplication(request);
        loop.exec();

        if (!networkError.isEmpty())
        {
            throw core::InfraException(networkError);
        }

        return response;
    }

    BackendClientAdapter::SendFriendApplicationErrCode BackendClientAdapter::fromSendErrCode(
        const QString& errCode)
    {
        if (errCode.isEmpty())
        {
            return SendFriendApplicationErrCode::None;
        }
        if (errCode == QString::fromLatin1(kErrCodeUserNotFound))
        {
            return SendFriendApplicationErrCode::UserNotFound;
        }
        if (errCode == QString::fromLatin1(kErrCodeDuplicateApplication))
        {
            return SendFriendApplicationErrCode::DuplicateApplication;
        }
        if (errCode == QString::fromLatin1(kErrCodeAlreadyFriend))
        {
            return SendFriendApplicationErrCode::AlreadyFriend;
        }
        if (errCode == QString::fromLatin1(kErrCodeSelfFriend))
        {
            return SendFriendApplicationErrCode::SelfFriend;
        }
        if (errCode == QString::fromLatin1(kErrCodeUnknownError))
        {
            return SendFriendApplicationErrCode::UnknownError;
        }

        return SendFriendApplicationErrCode::UnknownError;
    }

    OpenAPIChat::OAIChatAcceptFriendApplication_200_response ChatServerApiGateway::acceptFriendApplication(
        const OpenAPIChat::OAIChatAcceptFriendApplication_request& request,
        const QString& friendApplicationId)
    {
        OpenAPIChat::OAIChatAcceptFriendApplication_200_response response;

        OpenAPIChat::OAIChatDefaultApi api;
        api.setBearerToken(sys::common::component::UserCredentialManager::instance().getCurrentToken());
        QNetworkAccessManager manager;
        api.setNetworkAccessManager(&manager);
        api.setTimeOut(8000);
        QString networkError;

        QEventLoop loop;
        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::acceptFriendApplicationSignal,
                         &loop,
                         [&](const OpenAPIChat::OAIChatAcceptFriendApplication_200_response& summary)
                         {
                             response = summary;
                             loop.quit();
                         });

        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::acceptFriendApplicationSignalError,
                         &loop,
                         [&](const OpenAPIChat::OAIChatAcceptFriendApplication_200_response&,
                             QNetworkReply::NetworkError,
                             const QString& error)
                         {
                             networkError = error;
                             loop.quit();
                         });

        api.acceptFriendApplication(friendApplicationId, request);
        loop.exec();

        if (!networkError.isEmpty())
        {
            throw core::InfraException(networkError);
        }

        return response;
    }

    OpenAPIChat::OAIChatRejectFriendApplication_200_response ChatServerApiGateway::rejectFriendApplication(
        const QString& friendApplicationId)
    {
        OpenAPIChat::OAIChatRejectFriendApplication_200_response response;

        OpenAPIChat::OAIChatDefaultApi api;
        api.setBearerToken(sys::common::component::UserCredentialManager::instance().getCurrentToken());
        QNetworkAccessManager manager;
        api.setNetworkAccessManager(&manager);
        api.setTimeOut(8000);
        QString networkError;

        QEventLoop loop;
        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::rejectFriendApplicationSignal,
                         &loop,
                         [&](const OpenAPIChat::OAIChatRejectFriendApplication_200_response& summary)
                         {
                             response = summary;
                             loop.quit();
                         });

        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::rejectFriendApplicationSignalError,
                         &loop,
                         [&](const OpenAPIChat::OAIChatRejectFriendApplication_200_response&,
                             QNetworkReply::NetworkError,
                             const QString& error)
                         {
                             networkError = error;
                             loop.quit();
                         });

        api.rejectFriendApplication(friendApplicationId);
        loop.exec();

        if (!networkError.isEmpty())
        {
            throw core::InfraException(networkError);
        }

        return response;
    }

    BackendClientAdapter::HandleFriendApplicationErrCode BackendClientAdapter::fromHandleErrCode(
        const QString& errCode)
    {
        if (errCode.isEmpty())
        {
            return HandleFriendApplicationErrCode::None;
        }
        if (errCode == QString::fromLatin1(kErrCodeApplicationNotFound))
        {
            return HandleFriendApplicationErrCode::ApplicationNotFound;
        }
        if (errCode == QString::fromLatin1(kErrCodeApplicationAlreadyHandled))
        {
            return HandleFriendApplicationErrCode::ApplicationAlreadyHandled;
        }
        if (errCode == QString::fromLatin1(kErrCodeUnknownError))
        {
            return HandleFriendApplicationErrCode::UnknownError;
        }

        return HandleFriendApplicationErrCode::UnknownError;
    }

    port::SendFriendApplicationResult BackendClientAdapter::sendFriendApplication(
        const QString& targetUserId,
        const QString& verificationMessage,
        const QString& recipientRemark)
    {
        if (chatApiGateway == nullptr)
        {
            throw core::InfraException("ChatApiGateway 未配置");
        }

        OpenAPIChat::OAIChatSendFriendApplication_request request;
        request.setTargetUserId(targetUserId);
        request.setVerificationMessage(verificationMessage);
        request.setRecipientRemark(recipientRemark);

        const auto response = chatApiGateway->sendFriendApplication(request);
        if (!response.isSuccess())
        {
            switch (fromSendErrCode(response.getErrCode()))
            {
            case SendFriendApplicationErrCode::DuplicateApplication:
                throw domain::PendingFriendApplicationAlreadyExistsException();
            case SendFriendApplicationErrCode::AlreadyFriend:
                throw domain::FriendShipAlreadyExistsException();
            case SendFriendApplicationErrCode::SelfFriend:
                throw domain::ApplyFriendToMyselfException();
            case SendFriendApplicationErrCode::UserNotFound:
            case SendFriendApplicationErrCode::UnknownError:
            case SendFriendApplicationErrCode::None:
            default:
                throw core::InfraException(response.getErrMsg());
            }
        }

        if (!response.is_data_Set())
        {
            throw core::InfraException("好友申请响应缺少data");
        }

        return {response.getData().getFriendApplicationId(), QDateTime::currentDateTime()};
    }

    port::AcceptFriendApplicationResult BackendClientAdapter::acceptFriendApplication(
        const QString& friendApplicationId, const QString& remark)
    {
        if (chatApiGateway == nullptr)
        {
            throw core::InfraException("ChatApiGateway 未配置");
        }

        OpenAPIChat::OAIChatAcceptFriendApplication_request request;
        request.setRecipientRemark(remark);
        const auto response = chatApiGateway->acceptFriendApplication(request, friendApplicationId);
        if (!response.isSuccess())
        {
            switch (fromHandleErrCode(response.getErrCode()))
            {
            case HandleFriendApplicationErrCode::ApplicationNotFound:
                throw domain::FriendApplicationNotFoundException();
            case HandleFriendApplicationErrCode::ApplicationAlreadyHandled:
                throw domain::FriendApplicationAlreadyProcessedException();
            case HandleFriendApplicationErrCode::UnknownError:
            case HandleFriendApplicationErrCode::None:
            default:
                throw core::InfraException(response.getErrMsg());
            }
        }

        port::AcceptFriendApplicationResult result;
        result.isFriendShipCreated = false;
        if (!response.is_data_Set())
        {
            return result;
        }

        const auto data = response.getData();
        result.isFriendShipCreated = data.isCreateFriendShip();
        if (result.isFriendShipCreated)
        {
            result.newFriendShipInfo = port::NewFriendShipInfo{
                data.getFriendshipId(),
                data.getPrivateChatSessionId(),
                data.getApplicantParticipantId(),
                data.getTargetUserParticipantId()
            };
        }
        return result;
    }

    void BackendClientAdapter::rejectFriendApplication(const QString& friendApplicationId)
    {
        if (chatApiGateway == nullptr)
        {
            throw core::InfraException("ChatApiGateway 未配置");
        }

        const auto response = chatApiGateway->rejectFriendApplication(friendApplicationId);
        if (!response.isSuccess())
        {
            switch (fromHandleErrCode(response.getErrCode()))
            {
            case HandleFriendApplicationErrCode::ApplicationNotFound:
                throw domain::FriendApplicationNotFoundException();
            case HandleFriendApplicationErrCode::ApplicationAlreadyHandled:
                throw domain::FriendApplicationAlreadyProcessedException();
            case HandleFriendApplicationErrCode::UnknownError:
            case HandleFriendApplicationErrCode::None:
            default:
                throw core::InfraException(response.getErrMsg());
            }
        }
    }
}
