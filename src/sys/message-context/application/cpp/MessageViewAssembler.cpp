//
// Created by 86150 on 2026/4/2.
//

#include "../include/MessageViewAssembler.h"

#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"

namespace sys::message::application
{
    namespace
    {
        contract::message::MessageView toTextMessageView(const QSharedPointer<domain::Message>& message,
                                                         const common::port::UserInfo& sender,
                                                         const bool isMe)
        {
            if (message == nullptr)
            {
                throw core::InfraException("MessageViewAssembler: message为空");
            }

            const auto content = message->contentValue();
            if (content == nullptr)
            {
                throw core::InfraException("MessageViewAssembler: message content为空");
            }
            if (content->getContentType() != domain::ContentType::Text)
            {
                throw core::InfraException("MessageViewAssembler: 暂不支持该消息类型");
            }

            const auto* textContent = dynamic_cast<domain::TextContent*>(content.data());
            if (textContent == nullptr)
            {
                throw core::InfraException("MessageViewAssembler: 文本消息内容类型不匹配");
            }

            contract::message::MessageView view;
            view.msgId = message->messageIdValue();
            view.messageType = contract::message::MessageView::MessageType::Text;
            view.senderInfo.userId = sender.userId;
            view.senderInfo.nickname = sender.nickname;
            view.senderInfo.avatarFileId = sender.avatarFileId;
            view.isMe = isMe;
            view.timestamp = message->sendTimeValue();
            view.sequence = message->seqInChatSessionValue();
            view.content = textContent->textValue();
            return view;
        }
    }

    contract::message::MessageView MessageViewAssembler::assemble(
        const QSharedPointer<domain::Message>& message)
    {
        if (message == nullptr)
        {
            throw core::InfraException("MessageViewAssembler: message为空");
        }

        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        if (currentUserId == message->senderUserIdValue())
        {
            return assembleMyMessage(message);
        }
        else
        {
            return assembleOthersMessage(message);
        }
    }

    QList<contract::message::MessageView> MessageViewAssembler::assembleMany(
        const QList<QSharedPointer<domain::Message>>& messages)
    {
        if (messages.isEmpty())
        {
            return {};
        }

        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        QSet<QString> otherUserIds;
        for (const auto& message : messages)
        {
            if (message == nullptr)
            {
                throw core::InfraException("MessageViewAssembler: message为空");
            }
            const QString senderUserId = message->senderUserIdValue();
            if (senderUserId != currentUserId)
            {
                otherUserIds.insert(senderUserId);
            }
        }

        QHash<QString, common::port::UserInfo> users;
        if (!otherUserIds.isEmpty())
        {
            users = userClient->getUsers(otherUserIds);
        }

        QList<contract::message::MessageView> views;
        views.reserve(messages.size());
        for (const auto& message : messages)
        {
            const QString senderUserId = message->senderUserIdValue();
            if (senderUserId == currentUserId)
            {
                views.append(assembleMyMessage(message));
                continue;
            }

            common::port::UserInfo senderInfo;
            senderInfo.userId = senderUserId;
            if (users.contains(senderUserId))
            {
                senderInfo = users.value(senderUserId);
            }
            views.append(toTextMessageView(message, senderInfo, false));
        }
        return views;
    }

    contract::message::MessageView MessageViewAssembler::assembleMyMessage(
        const QSharedPointer<domain::Message>& message)
    {
        const QString senderUserId = message->senderUserIdValue();

        common::port::UserInfo userInfo;
        userInfo.userId = senderUserId;
        userInfo.nickname = common::component::UserCredentialManager::instance().getNickname();
        userInfo.avatarFileId = common::component::UserCredentialManager::instance().getAvatarFileId();
        return toTextMessageView(message, userInfo, true);
    }

    contract::message::MessageView MessageViewAssembler::assembleOthersMessage(
        const QSharedPointer<domain::Message>& message)
    {
        if (message == nullptr)
        {
            throw core::InfraException("MessageViewAssembler: message为空");
        }
        const QString senderUserId = message->senderUserIdValue();
        auto users = userClient->getUsers(QSet<QString>{senderUserId});

        common::port::UserInfo senderInfo;
        senderInfo.userId = senderUserId;
        if (users.contains(senderUserId))
        {
            senderInfo = users.value(senderUserId);
        }
        return toTextMessageView(message, senderInfo, false);
    }
}
