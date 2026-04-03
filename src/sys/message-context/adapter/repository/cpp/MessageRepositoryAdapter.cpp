//
// Created by 86150 on 2026/4/2.
//

#include "../include/MessageRepositoryAdapter.h"

#include <QSqlError>
#include <QSqlQuery>
#include <algorithm>

#include "sys/common/exception/InfraExcception.h"

namespace sys::message::adapter
{
    namespace
    {
        void ensureMessageTableReady(const QSqlDatabase& db)
        {
            if (!db.isOpen())
            {
                throw core::InfraException("数据库连接失败");
            }

            QSqlQuery query(db);
            if (!query.exec(
                "CREATE TABLE IF NOT EXISTS message ("
                "message_id TEXT PRIMARY KEY,"
                "chat_session_id TEXT NOT NULL,"
                "seq_in_chat_session INTEGER NOT NULL,"
                "send_time TEXT NOT NULL,"
                "sender_user_id TEXT NOT NULL,"
                "content_type INTEGER NOT NULL,"
                "text_content TEXT"
                ")"))
            {
                throw core::InfraException("消息表初始化失败: " + query.lastError().text());
            }
        }

        int contentTypeToInt(const domain::ContentType contentType)
        {
            switch (contentType)
            {
            case domain::ContentType::Text:
                return 0;
            case domain::ContentType::Photo:
                return 1;
            case domain::ContentType::Document:
                return 2;
            case domain::ContentType::Speech:
            default:
                return 3;
            }
        }

        domain::ContentType intToContentType(const int value)
        {
            switch (value)
            {
            case 0:
                return domain::ContentType::Text;
            case 1:
                return domain::ContentType::Photo;
            case 2:
                return domain::ContentType::Document;
            case 3:
            default:
                return domain::ContentType::Speech;
            }
        }
    }

    void MessageRepositoryAdapter::MessagePO::from(const QSharedPointer<domain::Message>& message)
    {
        if (message == nullptr)
        {
            throw core::InfraException("消息模型为空，无法持久化");
        }

        messageId = message->messageIdValue();
        chatSessionId = message->chatSessionIdValue();
        seqInChatSession = message->seqInChatSessionValue();
        sendTime = message->sendTimeValue().toString(Qt::ISODateWithMs);
        senderUserId = message->senderUserIdValue();

        const auto content = message->contentValue();
        if (content == nullptr)
        {
            throw core::InfraException("消息内容为空，无法持久化");
        }

        contentType = content->getContentType();
        if (contentType == domain::ContentType::Text)
        {
            const auto* textContent = dynamic_cast<domain::TextContent*>(content.data());
            if (textContent == nullptr)
            {
                throw core::InfraException("文本消息内容类型不匹配");
            }
            text = textContent->textValue();
            return;
        }

        throw core::InfraException("当前仅支持持久化文本消息");
    }

    void MessageRepositoryAdapter::MessagePO::from(const QSqlQuery& query)
    {
        messageId = query.value("message_id").toString();
        chatSessionId = query.value("chat_session_id").toString();
        seqInChatSession = query.value("seq_in_chat_session").toInt();
        sendTime = query.value("send_time").toString();
        senderUserId = query.value("sender_user_id").toString();
        contentType = intToContentType(query.value("content_type").toInt());
        if (!query.value("text_content").isNull())
        {
            text = query.value("text_content").toString();
        }
    }

    QSharedPointer<domain::Message> MessageRepositoryAdapter::MessagePO::toModel()
    {
        if (contentType != domain::ContentType::Text)
        {
            throw core::InfraException("当前仅支持反序列化文本消息");
        }
        if (!text.has_value())
        {
            throw core::InfraException("文本消息缺少文本内容");
        }

        QDateTime sendTimeValue = QDateTime::fromString(sendTime, Qt::ISODateWithMs);
        if (!sendTimeValue.isValid())
        {
            sendTimeValue = QDateTime::fromString(sendTime, Qt::ISODate);
        }
        if (!sendTimeValue.isValid())
        {
            throw core::InfraException("消息发送时间格式非法");
        }

        return domain::Message::ofTextMessage(messageId, chatSessionId, seqInChatSession, sendTimeValue,
                                              senderUserId, text.value());
    }

    void MessageRepositoryAdapter::save(const QSharedPointer<domain::Message> message)
    {
        MessagePO po;
        po.from(message);
        insertMessagePO(po);
    }

    void MessageRepositoryAdapter::saveAll(const QList<QSharedPointer<domain::Message>>& messages)
    {
        if (messages.isEmpty())
        {
            return;
        }

        QSqlDatabase db = privateDatabase->getDataBase();
        ensureMessageTableReady(db);

        if (!db.transaction())
        {
            throw core::InfraException("批量保存消息失败: 无法开启事务");
        }

        try
        {
            for (const auto& message : messages)
            {
                MessagePO po;
                po.from(message);
                insertMessagePO(po);
            }
            if (!db.commit())
            {
                throw core::InfraException("批量保存消息失败: 提交事务失败");
            }
        }
        catch (...)
        {
            db.rollback();
            throw;
        }
    }

    QSharedPointer<domain::Message> MessageRepositoryAdapter::of(const QString& messageId)
    {
        auto po = findMessagePOById(messageId);
        if (!po.has_value())
        {
            return nullptr;
        }
        return po->toModel();
    }

    QList<QSharedPointer<domain::Message>> MessageRepositoryAdapter::ofRecentMessages(const QString& chatSessionId,
        int count)
    {
        // 先保证表存在
        ensureMessageTableReady(privateDatabase->getDataBase());
        QList<QSharedPointer<domain::Message>> messages;
        auto pos = findRecentMessagePOsByChatSession(chatSessionId, count);
        messages.reserve(pos.size());
        for (auto& po : pos)
        {
            messages.append(po.toModel());
        }
        return messages;
    }

    void MessageRepositoryAdapter::insertMessagePO(const MessagePO& messagePO)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureMessageTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "INSERT OR REPLACE INTO message ("
            "message_id, chat_session_id, seq_in_chat_session, send_time, sender_user_id, content_type, text_content"
            ") VALUES ("
            ":message_id, :chat_session_id, :seq_in_chat_session, :send_time, :sender_user_id, :content_type, "
            ":text_content"
            ")");
        query.bindValue(":message_id", messagePO.messageId);
        query.bindValue(":chat_session_id", messagePO.chatSessionId);
        query.bindValue(":seq_in_chat_session", messagePO.seqInChatSession);
        query.bindValue(":send_time", messagePO.sendTime);
        query.bindValue(":sender_user_id", messagePO.senderUserId);
        query.bindValue(":content_type", contentTypeToInt(messagePO.contentType));
        query.bindValue(":text_content", messagePO.text.has_value() ? QVariant(messagePO.text.value()) : QVariant());

        if (!query.exec())
        {
            throw core::InfraException("保存消息失败: " + query.lastError().text());
        }
    }

    std::optional<MessageRepositoryAdapter::MessagePO> MessageRepositoryAdapter::findMessagePOById(
        const QString& messageId)
    {
        if (messageId.isEmpty())
        {
            return std::nullopt;
        }

        QSqlDatabase db = privateDatabase->getDataBase();
        ensureMessageTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT message_id, chat_session_id, seq_in_chat_session, send_time, sender_user_id, content_type, "
            "text_content "
            "FROM message WHERE message_id = :message_id LIMIT 1");
        query.bindValue(":message_id", messageId);

        if (!query.exec())
        {
            throw core::InfraException("按消息ID查询消息失败: " + query.lastError().text());
        }

        if (!query.next())
        {
            return std::nullopt;
        }

        MessagePO po;
        po.from(query);
        return po;
    }

    QList<MessageRepositoryAdapter::MessagePO> MessageRepositoryAdapter::findRecentMessagePOsByChatSession(
        const QString& chatSessionId, int count)
    {
        if (chatSessionId.isEmpty() || count <= 0)
        {
            return {};
        }

        QSqlDatabase db = privateDatabase->getDataBase();
        ensureMessageTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT message_id, chat_session_id, seq_in_chat_session, send_time, sender_user_id, content_type, "
            "text_content "
            "FROM message WHERE chat_session_id = :chat_session_id "
            "ORDER BY seq_in_chat_session DESC LIMIT :count");
        query.bindValue(":chat_session_id", chatSessionId);
        query.bindValue(":count", count);

        if (!query.exec())
        {
            throw core::InfraException("查询最近消息失败: " + query.lastError().text());
        }

        QList<MessagePO> rows;
        while (query.next())
        {
            MessagePO po;
            po.from(query);
            rows.append(po);
        }

        // DB按seq降序查出，返回给上层时按seq升序，便于聊天窗口正向渲染。
        std::reverse(rows.begin(), rows.end());
        return rows;
    }
}
