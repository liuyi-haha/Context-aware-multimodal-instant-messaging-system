//
// Created by 86150 on 2026/4/1.
//

#include "../include/MessageService.h"

#include "sys/common/component/UserCredentialManager.h"
#include "sys/message-context/domain/exception/InvalidTextException.h"
#include "sys/message-context/domain/object/include/Message.h"

namespace sys::message::domain
{
    QSharedPointer<Message> MessageService::sendTextMessage(const QString& chatSessionId, const QString& text)
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
        return message;
    }

    QList<QSharedPointer<Message>> MessageService::getRecentMessages(const QString& chatSessionId, int count)
    {
        if (count <= 0 || chatSessionId.isEmpty())
        {
            return {};
        }
        return messageRepository->ofRecentMessages(chatSessionId, count);
    }

    void MessageService::receiveTextMessage(const QString& sessionId, const QString& messageId,
                                            int seqInSession, const QString& senderUserId, const QString& textContent,
                                            const QDateTime& sendTime)
    {
        // 创建消息，保存消息
        auto message = Message::ofTextMessage(messageId, sessionId, seqInSession, sendTime, senderUserId, textContent);
        messageRepository->save(message);
    }

    QSharedPointer<Message> MessageService::getMessage(const QString& messageId)
    {
        return messageRepository->of(messageId);
    }
}
