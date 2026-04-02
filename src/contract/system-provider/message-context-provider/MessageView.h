//
// Created by 86150 on 2026/3/31.
//
#include <optional>
#include <QDateTime>
#include <QString>
#include <QSize>
#pragma once
namespace contract::message
{
    struct SenderInfo
    {
        QString userId;
        QString nickname;
        QString avatarFileId;
        std::optional<QByteArray> avatar;
    };

    struct MediaInfo
    {
        // 加载状态
        enum class LoadState
        {
            NotLoaded, // 未加载
            Loading, // 加载中
            Loaded, // 已加载
            Failed // 加载失败
        };

        QString fileId; // 文件标识
        QString fileName; // 文件名（文件用）
        qint64 fileSize; // 文件大小
        std::optional<QByteArray> data; // 数据缓存（图片/语音/文件）
        std::optional<int> duration; // 时长（语音用）
        std::optional<QSize> imageSize; // 图片尺寸（图片用）
        LoadState loadState = LoadState::NotLoaded;

        // 重载operator==，方便测试中断言
        bool operator==(const MediaInfo& other) const
        {
            return fileId == other.fileId &&
                fileName == other.fileName &&
                fileSize == other.fileSize &&
                data.has_value() == other.data.has_value() &&
                (!data.has_value() || data.value() == other.data.value()) &&
                duration == other.duration &&
                imageSize == other.imageSize &&
                loadState == other.loadState;
        }
    };

    struct MessageView
    {
        // 需要专门的装配器，因为要跨上下文获取信息
        enum class MessageType { Text, Image, File, Audio };


        QString msgId;
        MessageType messageType;
        SenderInfo senderInfo;
        bool isMe;
        QDateTime timestamp;
        int sequence;
        // 文本专用
        std::optional<QString> content;
        // 媒体专用（图片/文件/语音统一用这个）
        std::optional<MediaInfo> media;


        // 重载operator==，方便测试中断言
        bool operator==(const MessageView& other) const
        {
            return msgId == other.msgId &&
                messageType == other.messageType &&
                senderInfo.userId == other.senderInfo.userId &&
                senderInfo.nickname == other.senderInfo.nickname &&
                senderInfo.avatarFileId == other.senderInfo.avatarFileId &&
                isMe == other.isMe &&
                timestamp == other.timestamp &&
                sequence == other.sequence &&
                content == other.content &&
                media.has_value() == other.media.has_value() &&
                media == other.media;
        }
    };
}
