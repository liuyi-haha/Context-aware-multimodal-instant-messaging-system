//
// Created by 86150 on 2026/4/2.
//

#pragma once
#include <QString>

#include "MessageView.h"
#include "contract/BaseResponse.hpp"

namespace contract::message
{
    struct SendTextMessageRequest
    {
        QString chatSessionId;
        QString text;
    };

    struct SendTextMessageResponse : public BaseResponse
    {
        std::optional<MessageView> messageView; // 发送成功时返回的消息视图
    };
}
