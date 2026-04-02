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

        enum class MessageType
        {
            Text,
            Image,
            Document,
            Speech,
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
            const QSet<int>& chatSessionIds);

        // 如果第三个参数beforeMsgId有值，则获取在beforeMsgId之前的消息；如果没有值，则获取最新的消息
        QList<contract::message::MessageView> getMessagesBefore(const QString& chatSessionId, int count,
                                                                const std::optional<QString>& beforeMsgId =
                                                                    std::nullopt);
        contract::message::SendTextMessageResponse sendTextMessage(const QString& chatSessionId, const QString& text);

    private:
        MessageViewAssembler* messageViewAssembler = QInjection::Inject;
        domain::MessageService* messageService = QInjection::Inject;

    private:
        bool checkConfigAndSetResponse(contract::BaseResponse& response);
    };
}
