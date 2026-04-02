//
// Created by 86150 on 2026/3/30.
//

#pragma once
#include <QDateTime>
#include <QString>
#include <QObject>

namespace sys::relation::port
{
    class MessageClient : public QObject
    {
        Q_OBJECT

    public:
        enum class MessageType
        {
            Text,
            Image,
            Document,
            Speech,
            NoMessage // 用于表示没有消息的情况
        };

        struct LastMessageInfo
        {
            std::optional<QString> text; // 如果是文本消息，则为文本内容，否则为nullopt
            std::optional<QString> fileName; // 如果是文件消息，则为文件名（包含后缀），否则为nullopt
            MessageType type; // 消息类型，文本、图片、文件、语音等
            QDateTime sendTime;
        };

    public:
        virtual QHash<QString, LastMessageInfo> getLastMessageInfos(const QList<QString>& chatSessionIds) = 0;
    };
}
