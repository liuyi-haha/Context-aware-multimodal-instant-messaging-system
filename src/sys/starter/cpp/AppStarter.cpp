#include "../include/AppStarter.h"

#include "dependencypointer.h"
#include "dependencypool.h"
#include "sys/auth-context/starter/include/AuthContextStarter.h"
#include "sys/common/adapter/include/UserClientAdapter.h"
#include "sys/file-context/starter/include/FileContextStarter.h"
#include "sys/message-context/adapter/client/include/RelationClientAdapter.h"
#include "sys/message-context/starter/Starter.h"
#include "sys/notification-context/starter/include/NotificationContextStarter.h"
#include "sys/relation-context/starter/include/RelationContextStarter.h"
#include "sys/user-context/starter/include/UserContextStarter.h"

namespace sys::starter
{
    void bootstrapCommon()
    {
        QInjection::addSingleton(new common::adapter::UserClientAdapter);
    }

    void AppStarter::bootstrap()
    {
        static bool bootstrapped = false;
        if (bootstrapped)
        {
            return;
        }

        bootstrapCommon();

        // Keep initialization sequence explicit and centralized.
        file::starter::FileContextStarter::bootstrap();
        notification::starter::NotificationContextStarter::bootstrap();
        auth::starter::AuthContextStarter::bootstrap();
        user::starter::UserContextStarter::bootstrap();
        message::starter::Starter::bootstarp();
        // 一定要先初始化message上下文，本来规划的是relation上下文依赖message上下文，但现状是相互依赖
        // 所以规定先初始化message上下文，初始化relation上下文后再回过头来使用set依赖注入
        relation::starter::RelationContextStarter::bootstrap();

        // 处理循环依赖
        message::port::RelationClient* relationClientPtr = QInjection::Inject;

        reinterpret_cast<message::adapter::RelationClientAdapter*>(relationClientPtr)->setRelationApplicationService(
            QInjection::Inject);
        bootstrapped = true;
    }
}

