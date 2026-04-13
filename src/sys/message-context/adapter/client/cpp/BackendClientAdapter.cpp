//
// Created by 86150 on 2026/4/1.
//

#include "../include/BackendClientAdapter.h"

#include <QEventLoop>
#include <QNetworkAccessManager>

#include "OAIChatDefaultApi.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"
#include "sys/message-context/domain/exception/ChatSessionNotFoundException.h"
#include "sys/message-context/domain/exception/FriendShipNotFoundException.h"
#include "sys/message-context/domain/exception/InvalidTextException.h"
#include "sys/message-context/domain/exception/NotParticipantException.h"

namespace sys::message::adapter
{
    namespace
    {
        constexpr const char* kErrCodeSessionNotFound = "SESSION_NOT_FOUND";
        constexpr const char* kErrCodeFriendshipRequired = "FRIENDSHIP_REQUIRED";
        constexpr const char* kErrCodeNotParticipant = "NOT_PARTICIPANT";
        constexpr const char* kErrCodeContentEmpty = "CONTENT_EMPTY";
        constexpr const char* kErrCodeContentTooLong = "CONTENT_TOO_LONG";
        constexpr const char* kErrCodeUnknown = "UNKNOWN_ERROR";
    }

    BackendClientAdapter::SendTextMessageErrCode BackendClientAdapter::fromSendTextErrCode(const QString& errCode)
    {
        if (errCode.isEmpty())
        {
            return SendTextMessageErrCode::None;
        }
        if (errCode == QString::fromLatin1(kErrCodeSessionNotFound))
        {
            return SendTextMessageErrCode::SessionNotFound;
        }
        if (errCode == QString::fromLatin1(kErrCodeFriendshipRequired))
        {
            return SendTextMessageErrCode::FriendshipRequired;
        }
        if (errCode == QString::fromLatin1(kErrCodeNotParticipant))
        {
            return SendTextMessageErrCode::NotParticipant;
        }
        if (errCode == QString::fromLatin1(kErrCodeContentEmpty))
        {
            return SendTextMessageErrCode::ContentEmpty;
        }
        if (errCode == QString::fromLatin1(kErrCodeContentTooLong))
        {
            return SendTextMessageErrCode::ContentTooLong;
        }
        if (errCode == QString::fromLatin1(kErrCodeUnknown))
        {
            return SendTextMessageErrCode::UnknownError;
        }
        return SendTextMessageErrCode::UnknownError;
    }

    port::BackendClient::SendTextMessageResult BackendClientAdapter::sendTextMessage(
        const QString& chatSessionId, const QString& text)
    {
        if (chatApiGateway == nullptr)
        {
            throw core::InfraException("ChatApiGateway 未配置");
        }

        OpenAPIChat::OAIChatSendTextMessage_request request;
        request.setContent(text);

        const auto response = chatApiGateway->sendTextMessage(request, chatSessionId);
        if (!response.isSuccess())
        {
            switch (fromSendTextErrCode(response.getErrCode()))
            {
            case SendTextMessageErrCode::SessionNotFound:
                throw domain::ChatSessionNotFoundException();
            case SendTextMessageErrCode::FriendshipRequired:
                throw domain::FriendShipNotFoundException();
            case SendTextMessageErrCode::NotParticipant:
                throw domain::NotParticipantException();
            case SendTextMessageErrCode::ContentEmpty:
            case SendTextMessageErrCode::ContentTooLong:
                throw domain::InvalidTextException();
            case SendTextMessageErrCode::UnknownError:
            case SendTextMessageErrCode::None:
            default:
                throw core::InfraException(response.getErrMsg());
            }
        }

        if (!response.is_data_Set())
        {
            throw core::InfraException("发送文本消息响应缺少data");
        }

        const auto data = response.getData();
        return port::BackendClient::SendTextMessageResult{
            data.getMessageId(),
            data.getSendTime(),
            static_cast<int>(data.getSequence())
        };
    }

    OpenAPIChat::OAIChatSendTextMessage_200_response ChatApiGatewayAdapter::sendTextMessage(
        const OpenAPIChat::OAIChatSendTextMessage_request& request,
        const QString& chatSessionId)
    {
        OpenAPIChat::OAIChatSendTextMessage_200_response response;

        OpenAPIChat::OAIChatDefaultApi api;
        api.setBearerToken(common::component::UserCredentialManager::instance().getCurrentToken());
        QNetworkAccessManager manager;
        api.setNetworkAccessManager(&manager);
        api.setTimeOut(8000);
        QString networkError;

        QEventLoop loop;
        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::sendTextMessageSignal,
                         &loop,
                         [&](const OpenAPIChat::OAIChatSendTextMessage_200_response& summary)
                         {
                             response = summary;
                             loop.quit();
                         });

        QObject::connect(&api,
                         &OpenAPIChat::OAIChatDefaultApi::sendTextMessageSignalError,
                         &loop,
                         [&](const OpenAPIChat::OAIChatSendTextMessage_200_response&,
                             QNetworkReply::NetworkError,
                             const QString& error)
                         {
                             networkError = error;
                             loop.quit();
                         });

        api.sendTextMessage(chatSessionId, request);
        loop.exec();

        if (!networkError.isEmpty())
        {
            throw core::InfraException(networkError);
        }

        return response;
    }
}
