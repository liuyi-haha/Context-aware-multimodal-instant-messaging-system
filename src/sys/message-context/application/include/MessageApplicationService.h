//
// Created by 86150 on 2026/3/30.
//

#pragma once
#include <QDateTime>
#include <QString>

#include "dependencyinjector.h"
#include "MessageViewAssembler.h"
#include "contract/system-provider/message-context-provider/MessageView.h"
#include "contract/system-provider/message-context-provider/SendTextMessage.h"
#include "sys/message-context/domain/service/include/MessageService.h"

namespace sys::message::application
{
    class MessageApplicationService : public QObject
    {
        Q_OBJECT

    public:
        enum class MessageType
        {
            Text,
            Image,
            Document,
            Speech,
            NoMessage // 用于表示聊天会话没有消息的情况
        };

        struct LastMessageInfo
        {
            std::optional<QString> text; // 如果是文本消息，则为文本内容，否则为nullopt
            std::optional<QString> mediaFileId; // 如果是图片、文件或语音消息，则为媒体文件ID，否则为nullopt
            MessageType type; // 消息类型，文本、图片、文件、语音等
            QDateTime sendTime;
        };

    public:
        MessageApplicationService() = default;

        MessageApplicationService(MessageViewAssembler* messageAssembler, domain::MessageService* messageService)
            : messageViewAssembler(messageAssembler),
              messageService(messageService)
        {
        }

        QHash<QString, LastMessageInfo> getChatSessionLastMessageInfos(
            const QSet<QString>& chatSessionIds);

        // 获取聊天会话中最新的N消息——序号前N大的消息
        QList<contract::message::MessageView> getRecentMessages(const QString& chatSessionId, int count);
        // 获取聊天会话中seq序号之前的N条消息，返回的消息列表中不包含seq序号对应的消息
        QList<contract::message::MessageView> getMessagesBefore(const QString& chatSessionId, int count, int seq);
        contract::message::SendTextMessageResponse sendTextMessage(const QString& chatSessionId, const QString& text);
        void handleMessageReceived(const QByteArray& bytes);
        std::optional<contract::message::MessageView> getMessage(const QString& messageId);

    private:
        MessageViewAssembler* messageViewAssembler = QInjection::Inject;
        domain::MessageService* messageService = QInjection::Inject;

    private:
        bool checkConfigAndSetResponse(contract::BaseResponse& response);
    };
}
