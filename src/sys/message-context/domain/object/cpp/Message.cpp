//
// Created by 86150 on 2026/3/22.
//

#include "../include/Message.h"

namespace sys::message::domain
{
    QDateTime SendingInfo::sendTimeVal()
    {
        return sendTime;
    }

    QString SendingInfo::senderUserIdVal()
    {
        return senderUserId;
    }

    QSharedPointer<Content> ContentFactory::createTextContent(const QString& text)
    {
        return nullptr;
    }

    TextContent::TextContent(const QString& text)
    {
        // 如果不合法则抛异常
    }

    QSharedPointer<Message> Message::ofTextMessage(const QString& messageId, int seqInChatSession,
                                                   const QDateTime& sendTime, const QString& senderUserId,
                                                   const QString& text)
    {
        return nullptr;
    }

    QString Message::messageIdValue()
    {
        return messageId;
    }

    int Message::seqInChatSessionValue()
    {
        return seqInChatSession;
    }

    QDateTime Message::sendTimeValue()
    {
        return sendingInfo.sendTimeVal();
    }

    QString Message::senderUserIdValue()
    {
        return sendingInfo.senderUserIdVal();
    }

    QSharedPointer<Content> Message::contentValue()
    {
        return content;
    }


    Message::Message(const QString& messageId, int seqInChatSession, const SendingInfo& sendingInfo,
                     const QSharedPointer<Content>& content)
        : messageId(messageId),
          seqInChatSession(seqInChatSession),
          sendingInfo(sendingInfo),
          content(content)
    {
    }

    ContentType TextContent::getContentType()
    {
        return ContentType::Text;
    }

    bool TextContent::checkText(const QString& text)
    {
        return false;
    }

    QSharedPointer<TextContent> TextContent::of(const QString& text)
    {
        return nullptr;
    }

    QString TextContent::textValue()
    {
        return text;
    }
}
