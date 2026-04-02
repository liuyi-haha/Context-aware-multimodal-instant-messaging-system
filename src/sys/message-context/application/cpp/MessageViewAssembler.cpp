//
// Created by 86150 on 2026/4/2.
//

#include "../include/MessageViewAssembler.h"

#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"

namespace sys::message::application
{
    contract::message::MessageView MessageViewAssembler::assemble(
        const QSharedPointer<domain::Message>& message)
    {
        if (message == nullptr)
        {
            throw core::InfraException("MessageViewAssembler: message为空");
        }

        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        const QString senderUserId = message->senderUserIdValue();

        contract::message::MessageView view;
        view.msgId = message->messageIdValue();
        view.senderInfo.userId = senderUserId;
        view.senderInfo.nickname = common::component::UserCredentialManager::instance().getNickname();
        view.senderInfo.avatarFileId = common::component::UserCredentialManager::instance().getAvatarFileId();
        view.isMe = (senderUserId == currentUserId);
        view.timestamp = message->sendTimeValue();
        view.sequence = message->seqInChatSessionValue();

        const auto content = message->contentValue();
        if (content == nullptr)
        {
            throw core::InfraException("MessageViewAssembler: message content为空");
        }

        if (content->getContentType() == domain::ContentType::Text)
        {
            view.messageType = contract::message::MessageView::MessageType::Text;
            const auto* textContent = dynamic_cast<domain::TextContent*>(content.data());
            if (textContent == nullptr)
            {
                throw core::InfraException("MessageViewAssembler: 文本消息内容类型不匹配");
            }
            view.content = textContent->textValue();
            return view;
        }

        throw core::InfraException("MessageViewAssembler: 暂不支持该消息类型");
    }
}
