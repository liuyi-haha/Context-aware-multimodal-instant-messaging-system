//
// Created by 86150 on 2026/3/22.
//

#include "../include/Message.h"

#include "sys/message-context/domain/exception/InvalidTextException.h"

namespace sys::message::domain
{
    SendingInfo::SendingInfo(const QDateTime& sendTime, const QString& senderUserId)
        : sendTime(sendTime),
          senderUserId(senderUserId)
    {
    }

    QDateTime SendingInfo::sendTimeVal() const
    {
        return sendTime;
    }

    QString SendingInfo::senderUserIdVal() const
    {
        return senderUserId;
    }

    QSharedPointer<Content> ContentFactory::createTextContent(const QString& text)
    {
        return TextContent::of(text);
    }

    TextContent::TextContent(const QString& text)
        : text(text)
    {
        if (!checkText(text))
        {
            throw InvalidTextException();
        }
    }

    QSharedPointer<Message> Message::ofTextMessage(const QString& messageId, int seqInChatSession,
                                                   const QDateTime& sendTime, const QString& senderUserId,
                                                   const QString& text)
    {
        const SendingInfo sendingInfo(sendTime, senderUserId);
        const auto content = ContentFactory::createTextContent(text);
        return QSharedPointer<Message>(new Message(messageId, seqInChatSession, sendingInfo, content));
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
        return !text.isEmpty() && text.size() <= 1000;
    }

    QSharedPointer<TextContent> TextContent::of(const QString& text)
    {
        return QSharedPointer<TextContent>(new TextContent(text));
    }

    QString TextContent::textValue() const
    {
        return text;
    }
}
