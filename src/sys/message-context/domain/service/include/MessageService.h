//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "dependencyinjector.h"
#include "SenderValidator.h"
#include "sys/message-context/port/client/BackendClient.h"
#include "sys/message-context/port/repository/MessageRepository.h"
#include <QObject>

namespace sys::message::domain
{
    class MessageService : public QObject
    {
        Q_OBJECT

    public:
        MessageService() = default;

        explicit MessageService(port::BackendClient* backendClient, SenderValidator* senderValidator,
                                port::MessageRepository* messageRepository)
            : backendClient(backendClient),
              senderValidator(senderValidator),
              messageRepository(messageRepository)
        {
        }

    public:
        QSharedPointer<Message> sendTextMessage(const QString& chatSessionId, const QString& text);
        QList<QSharedPointer<Message>> getRecentMessages(const QString& chatSessionId, int count);
        void receiveTextMessage(const QString& sessionId, const QString& messageId, int seqInSession,
                                const QString& senderUserId, const QString& textContent, const QDateTime& sendTime);
        QSharedPointer<Message> getMessage(const QString& messageId);

    private:
        port::BackendClient* backendClient = QInjection::Inject;
        SenderValidator* senderValidator = QInjection::Inject;
        port::MessageRepository* messageRepository = QInjection::Inject;
    };
}
