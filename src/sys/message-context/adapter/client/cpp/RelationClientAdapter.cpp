//
// Created by 86150 on 2026/4/1.
//

#include "../include/RelationClientAdapter.h"

#include "sys/common/exception/InfraExcception.h"

namespace sys::message::adapter
{
    void RelationClientAdapter::checkSenderHasPermissionToSendMessage(const QString& chatSessionId)
    {
        checkConfig();
        relationApplicationService->checkSenderHasPermissionToSendMessage(chatSessionId);
    }

    void RelationClientAdapter::checkConfig()
    {
        if (relationApplicationService == nullptr)
        {
            throw core::InfraException("RelationApplicationService is not set in RelationClientAdapter");
        }
    }
}
