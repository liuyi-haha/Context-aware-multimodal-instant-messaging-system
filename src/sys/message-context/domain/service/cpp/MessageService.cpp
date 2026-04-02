//
// Created by 86150 on 2026/4/1.
//

#include "../include/MessageService.h"

#include "sys/common/component/UserCredentialManager.h"
#include "sys/message-context/domain/exception/InvalidTextException.h"
#include "sys/message-context/domain/object/include/Message.h"

namespace sys::message::domain
{
    void MessageService::sendTextMessage(const QString& chatSessionId, const QString& text)
    {
        if (!TextContent::checkText(text))
        {
            throw InvalidTextException();
        }
        senderValidator->checkSenderHasPermissionToSendMessage(chatSessionId);

        const auto result = backendClient->sendTextMessage(chatSessionId, text);
        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        const auto message = Message::ofTextMessage(result.messageId, chatSessionId,
                                                    result.seqInChatSession, result.sendTime,
                                                    currentUserId, text);
        messageRepository->save(message);
    }
}
