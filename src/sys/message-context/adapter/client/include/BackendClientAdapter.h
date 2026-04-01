//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "OAIChatSendTextMessage_200_response.h"
#include "OAIChatSendTextMessage_request.h"
#include "sys/message-context/port/client/include/BackendClient.h"

namespace sys::message::adapter
{
    class BackendClientAdapter : public port::BackendClient
    {
    public:
        void sendTextMessage(const QString& chatSessionId, const QString& text) override;
    };

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
}
