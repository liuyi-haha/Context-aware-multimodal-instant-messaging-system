//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <QObject>

#include "sys/message-context/domain/object/include/Message.h"

namespace sys::message::port
{
    class BackendClient : public QObject
    {
        Q_OBJECT

    public:
        struct SendTextMessageResult
        {
            QString messageId;
            QDateTime sendTime;
            int seqInChatSession; // 消息在聊天会话中的序号，可以用来保证消息的顺序性
        };

        virtual SendTextMessageResult sendTextMessage(const QString& chatSessionId, const QString& text) = 0;
    };
}
