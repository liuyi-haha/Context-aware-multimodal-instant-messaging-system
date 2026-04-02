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
        virtual OpenAPIChat::OAIChatSendTextMessage_200_response sendTextMessage(
            const OpenAPIChat::OAIChatSendTextMessage_request& request) = 0;
    };

    class ChatApiGatewayAdapter : public IChatApiGateway
    {
    public:
        OpenAPIChat::OAIChatSendTextMessage_200_response sendTextMessage(
            const OpenAPIChat::OAIChatSendTextMessage_request& request) override;
    };

    class BackendClientAdapter : public port::BackendClient
    {
    public:
        BackendClientAdapter() = default;

        explicit BackendClientAdapter(IChatApiGateway* chatApiGateway)
            : chatApiGateway(chatApiGateway)
        {
        }

        SendTextMessageResult sendTextMessage(const QString& chatSessionId, const QString& text) override;

    private:
        IChatApiGateway* chatApiGateway = new ChatApiGatewayAdapter();
    };
}
