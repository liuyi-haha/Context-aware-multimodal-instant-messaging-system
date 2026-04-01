//
// Created by 86150 on 2026/3/30.
//

#include "../include/MessageApplicationService.h"

namespace sys
{
    namespace message::application
    {
        QHash<QString, MessageApplicationService::LastMessageInfo> MessageApplicationService::
        getChatSessionLastMessageInfos(const QSet<int>& chatSessionIds)
        {
            // todo @liuyi
            // 构造一些mock数据
            QHash<QString, LastMessageInfo> infos;
            for (int chatSessionId : chatSessionIds)
            {
                LastMessageInfo info;
                info.text = QString("这是聊天会话 %1 的最后一条消息").arg(chatSessionId);
                info.mediaFileId = std::nullopt;
                info.type = MessageType::Text;
                info.sendTime = QDateTime::currentDateTime().addSecs(-chatSessionId * 60); // 模拟不同的发送时间
            }
            return infos;
        }

        QList<contract::message::MessageView> MessageApplicationService::getMessagesBefore(
            const QString& chatSessionId, int count, const std::optional<QString>& beforeMsgId)
        {
            // todo @liuyi
            // mock一些数据
            QList<contract::message::MessageView> messages;
            for (int i = 0; i < count; ++i)
            {
                contract::message::SenderInfo senderInfo = {
                    .userId = QString("user%1").arg(i % 5),
                    .nickname = QString("用户%1").arg(i % 5),
                    .avatarFileId = QString("avatar%1").arg(i % 5)
                };

                contract::message::MessageView msg;
                msg.senderInfo = senderInfo;
                msg.messageType = contract::message::MessageView::MessageType::Text;
                msg.msgId = QString("%1").arg(beforeMsgId.has_value() ? beforeMsgId->toInt() - i - 1 : 1000 - i);
                // 模拟消息ID递减
                msg.senderInfo.userId = QString("user%1").arg(i % 5);
                msg.isMe = (i % 5 == 0); // 模拟部分消息是自己发送的
                msg.timestamp = QDateTime::currentDateTime().addSecs(-i * 60);
                msg.sequence = beforeMsgId.has_value() ? beforeMsgId->toInt() - i - 1 : 1000 - i; // 模拟sequence递减
                msg.content = QString("这是消息 %1 的内容").arg(msg.msgId);

                messages.append(msg);
            }

            return messages;
        }
    }
} // sys
