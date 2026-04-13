//
// Created by 86150 on 2026/3/30.
//

#include "../include/MessageApplicationService.h"

#include "notifications.pb.h"
#include "sys/message-context/domain/object/include/Message.h"
#include "ui/common/UIEVentBus.h"

namespace sys
{
    namespace message::application
    {
        namespace
        {
            MessageApplicationService::MessageType toMessageType(const sys::message::domain::ContentType contentType)
            {
                switch (contentType)
                {
                case sys::message::domain::ContentType::Text:
                    return MessageApplicationService::MessageType::Text;
                case sys::message::domain::ContentType::Photo:
                    return MessageApplicationService::MessageType::Image;
                case sys::message::domain::ContentType::Document:
                    return MessageApplicationService::MessageType::Document;
                case sys::message::domain::ContentType::Speech:
                    return MessageApplicationService::MessageType::Speech;
                default:
                    return MessageApplicationService::MessageType::NoMessage;
                }
            }

            MessageApplicationService::LastMessageInfo toLastMessageInfo(
                const QSharedPointer<sys::message::domain::Message>& message)
            {
                MessageApplicationService::LastMessageInfo info;
                if (message == nullptr)
                {
                    info.type = MessageApplicationService::MessageType::NoMessage;
                    info.sendTime = QDateTime();
                    return info;
                }

                info.sendTime = message->sendTimeValue();
                const auto content = message->contentValue();
                if (content == nullptr)
                {
                    info.type = MessageApplicationService::MessageType::NoMessage;
                    return info;
                }

                info.type = toMessageType(content->getContentType());
                if (content->getContentType() == sys::message::domain::ContentType::Text)
                {
                    const auto* textContent = dynamic_cast<sys::message::domain::TextContent*>(content.data());
                    if (textContent != nullptr)
                    {
                        info.text = textContent->textValue();
                    }
                }
                return info;
            }
        }

        QHash<QString, MessageApplicationService::LastMessageInfo> MessageApplicationService::
        getChatSessionLastMessageInfos(const QSet<QString>& chatSessionIds)
        {
            QHash<QString, LastMessageInfo> infos;
            if (messageService == nullptr)
            {
                return infos;
            }

            for (const auto& chatSessionId : chatSessionIds)
            {
                const auto messages = messageService->getRecentMessages(chatSessionId, 1);
                if (messages.isEmpty())
                {
                    LastMessageInfo info;
                    info.type = MessageType::NoMessage;
                    info.sendTime = QDateTime();
                    infos.insert(chatSessionId, info);
                    continue;
                }

                infos.insert(chatSessionId, toLastMessageInfo(messages.constLast()));
            }
            return infos;
        }

        QList<contract::message::MessageView> MessageApplicationService::getRecentMessages(
            const QString& chatSessionId, int count)
        {
            if (messageService == nullptr || messageViewAssembler == nullptr)
            {
                return {};
            }

            const auto messages = messageService->getRecentMessages(chatSessionId, count);
            return messageViewAssembler->assembleMany(messages);
        }

        contract::message::SendTextMessageResponse MessageApplicationService::sendTextMessage(
            const QString& chatSessionId, const QString& text)
        {
            contract::message::SendTextMessageResponse response;
            checkConfigAndSetResponse(response);
            try
            {
                auto message = messageService->sendTextMessage(chatSessionId, text);
                response.messageView = messageViewAssembler->assemble(message);
                response.success = true;
            }
            catch (std::exception& ex)
            {
                response.success = false;
                response.errMsg = QString::fromUtf8(ex.what());
            }
            return response;
        }

        void MessageApplicationService::handleMessageReceived(const QByteArray& bytes)
        {
            // 先反序列化
            try
            {
                // 先反序列化
                proto::notification::MessageReceivedNotification notification;
                assert(notification.ParseFromArray(bytes.data(), bytes.size()));
                // todo @liuyi 后续扩展其它类型
                assert(notification.message_type() == proto::notification::MESSAGE_TYPE_TEXT);


                QString sessionId = QString::fromStdString(notification.session_id());
                QString messageId = QString::fromStdString(notification.message_id());
                int seqInSession = notification.seq_in_session();
                QString senderUserId = QString::fromStdString(notification.sender_user_id());
                QString textContent = QString::fromStdString(notification.text_content());
                QDateTime sendTime = QDateTime::fromString(QString::fromStdString(notification.send_time()),
                                                           Qt::ISODate);
                // 再调用领域服务的方法
                messageService->receiveTextMessage(sessionId, messageId, seqInSession, senderUserId, textContent,
                                                   sendTime);

                // 发送信号，通知消息列表更新
                qDebug() << "MessageApplicationService::handleMessageReceived: 收到消息通知，发送messageReceived信号，sessionId:" <<
                    sessionId << "messageId:" << messageId << "senderUserId:" << senderUserId << "textContent:" <<
                    textContent;
                emit ui::common::UIEventBus::instance()->messageReceived(sessionId, messageId);
                // 发送信号，通知ChatSessionList更新会话列表
                emit ui::common::UIEventBus::instance()->chatSessionUpdated(sessionId);
            }
            catch (const std::exception& ex)
            {
                qDebug() << "处理好友申请通知时发生异常:" << QString::fromStdString(ex.what());
            }
        }

        std::optional<contract::message::MessageView> MessageApplicationService::getMessage(const QString& messageId)
        {
            auto message = messageService->getMessage(messageId);
            if (message == nullptr)
            {
                qDebug() << "MessageService say: message not found for id:" << messageId;
                return std::nullopt;
            }
            return messageViewAssembler->assemble(message);
        }

        bool MessageApplicationService::checkConfigAndSetResponse(contract::BaseResponse& response)
        {
            if (messageViewAssembler == nullptr || messageService == nullptr)
            {
                response.success = false;
                response.errMsg = "MessageApplicationService is not properly configured";
                return false;
            }
            return true;
        }
    }
} // sys
