//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "dependencyinjector.h"
#include "SenderValidator.h"
#include "sys/message-context/port/client/BackendClient.h"
#include "sys/message-context/port/repository/MessageRepository.h"

namespace sys::message::domain
{
    class MessageService
    {
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
        void sendTextMessage(const QString& chatSessionId, const QString& text);

    private:
        port::BackendClient* backendClient = QInjection::Inject;
        SenderValidator* senderValidator = QInjection::Inject;
        port::MessageRepository* messageRepository = QInjection::Inject;
    };
}
