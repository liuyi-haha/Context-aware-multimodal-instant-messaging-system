//
// Created by 86150 on 2026/3/30.
//

#pragma once
#include "sys/notification-context/application/service/include/NotificationApplicationService.h"
#include "sys/relation-context/port/client/include/NotificationClient.h"

namespace sys::relation::adapter
{
    class NotificationClientAdapter : public port::NotificationClient
    {
    public:
        explicit NotificationClientAdapter(
            notification::application::NotificationApplicationService* notificationApplicationService =
                QInjection::Inject)
            : notificationApplicationService(notificationApplicationService)
        {
        }

        int getUnreadChatSessionItemCount(const QString& chatSessionId) override;

    private:
        notification::application::NotificationApplicationService* notificationApplicationService = nullptr;
    };
}
