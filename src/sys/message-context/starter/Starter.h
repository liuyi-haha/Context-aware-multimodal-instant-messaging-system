//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include "dependencypool.h"
#include "sys/common/adapter/include/UserClientAdapter.h"
#include "sys/message-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/message-context/adapter/client/include/RelationClientAdapter.h"
#include "sys/message-context/adapter/repository/include/MessageRepositoryAdapter.h"
#include "sys/message-context/application/include/MessageApplicationService.h"

namespace sys::message::starter
{
    class Starter
    {
    public:
        static void bootstarp()
        {
            // adapter
            QInjection::addSingleton(new adapter::MessageRepositoryAdapter);
            // 这里并没有完成初始化，后续通过set方法注入RelationApplicationService
            QInjection::addSingleton(new adapter::RelationClientAdapter);
            QInjection::addSingleton(new adapter::BackendClientAdapter);

            // 领域服务
            QInjection::addSingleton(new domain::SenderValidator);
            QInjection::addSingleton(new domain::MessageService);

            // 装配器
            QInjection::addSingleton(new application::MessageViewAssembler);

            // 应用服务
            QInjection::addSingleton(new application::MessageApplicationService);
        }
    };
};
