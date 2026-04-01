//
// Created by 86150 on 2026/3/27.
//

#pragma once
#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <optional>

namespace contract::relation
{
    enum class ChatSessionType
    {
        Private,
        Group
    };

    struct ChatSessionView
    {
        QString chatSessionId; // 聊天会话ID
        ChatSessionType chatSessionType; // "Private"或"Group"
        QString name; // 如果是单聊会话，则为好友的昵称; 如果是群聊会话，则为群名称
        QString avatarFileId; // 同上，单聊会话为好友头像文件ID，群聊会话为群头像文件ID
        std::optional<QByteArray> avatar; // 头像数据，可能为空
        // 最后一条消息的摘要, 如果是文本消息，则为文本内容；若为文件消息，则为"[文件] 文件名.ext"；如果是图片消息，则为"[图片]"；如果是语音消息，则为"[语音]"
        QString lastMessageSummary;
        QDateTime lastMessageSendTime; // 最后一条消息的发送时间
        int unreadCount; // 会话中未读消息的数量
    };
}
