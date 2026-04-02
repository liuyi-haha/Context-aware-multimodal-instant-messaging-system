//
// Created by 86150 on 2026/4/2.
//

#pragma once
#include "sys/common/component/Database.h"
#include "sys/message-context/port/repository/MessageRepository.h"

namespace sys::message::adapter
{
    class MessageRepositoryAdapter : public port::MessageRepository
    {
        struct MessagePO
        {
            QString messageId;
            QString chatSessionId;
            int seqInChatSession;
            QString sendTime; //ISO 8601格式的字符串
            QString senderUserId;
            std::optional<QString> text; // 暂时只支持文本消息
            domain::ContentType contentType;

            void from(const QSharedPointer<domain::Message>& message);
            void from(const QSqlQuery& query);
            QSharedPointer<domain::Message> toModel();
        };

    public:
        explicit MessageRepositoryAdapter(common::component::IPrivateDatabase* privateDatabase)
            : privateDatabase(privateDatabase)
        {
        }

        void save(const QSharedPointer<domain::Message>) override;
        QSharedPointer<domain::Message> of(const QString& messageId) override;

    private:
        common::component::IPrivateDatabase* privateDatabase = common::component::PrivateDatabase::instance();

    private:
        // 对MessagePO进行增删改查的函数
        void insertMessagePO(const MessagePO& messagePO);
        std::optional<MessagePO> findMessagePOById(const QString& messageId);
    };
}
