//
// Created by 86150 on 2026/3/30.
//

#pragma once
#include "dependencyinjector.h"
#include "sys/message-context/application/include/MessageApplicationService.h"
#include "sys/relation-context/port/client/include/MessageClient.h"

namespace sys::relation::adapter
{
    class MessageClientAdapter : public port::MessageClient
    {
    public:
        explicit MessageClientAdapter(
            message::application::MessageApplicationService* messageApplicationService = QInjection::Inject)
            : messageApplicationService(messageApplicationService)
        {
        }

        QHash<QString, LastMessageInfo>
        getLastMessageInfos(const QList<QString>& chatSessionIds) override;

    private:
        message::application::MessageApplicationService* messageApplicationService = nullptr;
    };
}
