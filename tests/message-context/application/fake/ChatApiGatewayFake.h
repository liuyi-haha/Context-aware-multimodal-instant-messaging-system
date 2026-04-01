//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/message-context/adapter/client/include/BackendClientAdapter.h"

class QString;

namespace tests::message::fake
{
    class ChatApiGatewayFake : public sys::message::adapter::IChatApiGateway
    {
    public:
        OpenAPIChat::OAIChatSendTextMessage_200_response ChatApiGatewayAdapter::sendTextMessage(
            const OpenAPIChat::OAIChatSendTextMessage_request& request) override
        {
            OpenAPIChat::OAIChatSendTextMessage_200_response response;
            response.statusCode = 200;
            return response;
        }
    };
}
