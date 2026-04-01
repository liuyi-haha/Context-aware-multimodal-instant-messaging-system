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
        QString fileId; // 文件标识
        QString fileName; // 文件名（文件用）
        qint64 fileSize; // 文件大小
        std::optional<QByteArray> data; // 数据缓存（图片/语音/文件）
        std::optional<int> duration; // 时长（语音用）
        std::optional<QSize> imageSize; // 图片尺寸（图片用）

        // 加载状态
        enum class LoadState
        {
            NotLoaded, // 未加载
            Loading, // 加载中
            Loaded, // 已加载
            Failed // 加载失败
        };

        LoadState loadState = LoadState::NotLoaded;
    };

    struct MessageView
    {
    public:
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
    };
}
