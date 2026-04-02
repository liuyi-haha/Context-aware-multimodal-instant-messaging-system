//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "../../../port/client/RelationClient.h"
#include "sys/relation-context/application/service/include/RelationApplicationService.h"

namespace sys::message::adapter
{
    class RelationClientAdapter : public port::RelationClient
    {
    public:
        RelationClientAdapter() = default; // 不要在构造函数里注入RelationApplicationService，因为relation上下文依赖message上下文，构成循环依赖

        void checkSenderHasPermissionToSendMessage(const QString& chatSessionId) override;

        void setRelationApplicationService(relation::application::RelationApplicationService* service)
        {
            relationApplicationService = service;
        }

    private:
        void checkConfig();
        relation::application::RelationApplicationService* relationApplicationService;
    };
}
