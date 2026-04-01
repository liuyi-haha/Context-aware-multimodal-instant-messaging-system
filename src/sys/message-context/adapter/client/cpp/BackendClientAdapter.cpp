//
// Created by 86150 on 2026/4/1.
//

#include "../include/BackendClientAdapter.h"

namespace sys::message::adapter
{
    void BackendClientAdapter::sendTextMessage(const QString& chatSessionId, const QString& text)
    {
    }

    OpenAPIChat::OAIChatSendTextMessage_200_response ChatApiGatewayAdapter::sendTextMessage(
        const OpenAPIChat::OAIChatSendTextMessage_request& request)
    {
    }
}
