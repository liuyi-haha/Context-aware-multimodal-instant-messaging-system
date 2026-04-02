//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "OAIChatSendTextMessage_200_response.h"
#include "OAIChatSendTextMessage_request.h"
#include "sys/message-context/port/client/BackendClient.h"

namespace sys::message::adapter
{
    class IChatApiGateway
    {
    public:
        virtual ~IChatApiGateway() = default;
        virtual OpenAPIChat::OAIChatSendTextMessage_200_response sendTextMessage(
            const OpenAPIChat::OAIChatSendTextMessage_request& request,
            const QString& chatSessionId) = 0;
    };

    class ChatApiGatewayAdapter : public IChatApiGateway
    {
    public:
        OpenAPIChat::OAIChatSendTextMessage_200_response sendTextMessage(
            const OpenAPIChat::OAIChatSendTextMessage_request& request,
            const QString& chatSessionId) override;
    };

    class BackendClientAdapter : public port::BackendClient
    {
    public:
        enum class SendTextMessageErrCode
        {
            None,
            SessionNotFound,
            FriendshipRequired,
            NotParticipant,
            ContentEmpty,
            ContentTooLong,
            UnknownError
        };

    public:
        explicit BackendClientAdapter(IChatApiGateway* chatApiGateway = new ChatApiGatewayAdapter())
            : chatApiGateway(chatApiGateway)
        {
        }

        SendTextMessageResult sendTextMessage(const QString& chatSessionId, const QString& text) override;

    private:
        static SendTextMessageErrCode fromSendTextErrCode(const QString& errCode);

    private:
        IChatApiGateway* chatApiGateway = nullptr;
    };
}
