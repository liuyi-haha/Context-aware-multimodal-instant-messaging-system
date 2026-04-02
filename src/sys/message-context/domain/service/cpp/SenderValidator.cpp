//
// Created by 86150 on 2026/4/1.
//

#include "../include/SenderValidator.h"

namespace sys::message::domain
{
    void SenderValidator::checkSenderHasPermissionToSendMessage(const QString& chatSessionId)
    {
        relationClient->checkSenderHasPermissionToSendMessage(chatSessionId);
        // 如果没有权限则抛异常
    }
}
