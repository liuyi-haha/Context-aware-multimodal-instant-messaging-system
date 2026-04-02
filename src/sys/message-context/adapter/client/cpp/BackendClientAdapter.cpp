//
// Created by 86150 on 2026/4/1.
//

#include "../include/BackendClientAdapter.h"

namespace sys::message::adapter
{
    port::BackendClient::SendTextMessageResult BackendClientAdapter::sendTextMessage(
        const QString& chatSessionId, const QString& text)
    {
        // 1. 构建request
        // 2. 调用OpenAPIChat::OAIChatSendTextMessage
        // 3. 解析response，如果失败则根据errCode抛异常，如果成功则将response转换为SendTextMessageResult并返回
    }

    OpenAPIChat::OAIChatSendTextMessage_200_response ChatApiGatewayAdapter::sendTextMessage(
        const OpenAPIChat::OAIChatSendTextMessage_request& request)
    {
    }
}
