//
// Created by 86150 on 2026/3/30.
//

#include "../include/NotificationClientAdapter.h"

namespace sys::relation::adapter
{
    int NotificationClientAdapter::getUnreadChatSessionItemCount(const QString& chatSessionId)
    {
        return notificationApplicationService->getChatSessionItemCount(chatSessionId);
    }
}
