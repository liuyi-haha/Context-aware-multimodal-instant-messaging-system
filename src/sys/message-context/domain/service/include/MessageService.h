//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "dependencyinjector.h"
#include "SenderValidator.h"
#include "sys/message-context/port/client/include/BackendClient.h"

namespace sys::message::domain
{
    class MessageService
    {
    public:
        explicit MessageService(port::BackendClient* backendClient)
            : backendClient(backendClient)
        {
        }

    public:
        void sendTextMessage(const QString& chatSessionId, const QString& text);

    private:
        port::BackendClient* backendClient = QInjection::Inject;
        SenderValidator* senderValidator = QInjection::Inject;
    };
}
