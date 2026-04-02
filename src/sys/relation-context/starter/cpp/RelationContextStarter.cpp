#include "../include/RelationContextStarter.h"

#include "dependencypool.h"

#include "sys/relation-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/relation-context/adapter/client/include/NotificationClientAdapter.h"
#include "sys/common/adapter/include/UserClientAdapter.h"
#include "sys/relation-context/adapter/client/include/MessageClientAdapter.h"
#include "sys/relation-context/adapter/repository/include/FriendApplicationRepositoryAdapter.h"
#include "sys/relation-context/adapter/repository/include/FriendRelationRepositoryAdapter.h"
#include "sys/relation-context/adapter/repository/include/ParticipantRepositoryAdapter.h"
#include "sys/relation-context/adapter/repository/include/PrivateChatSessionRepositoryAdapter.h"
#include "sys/relation-context/application/service/include/RelationApplicationService.h"
#include "sys/relation-context/application/assembler/include/FriendApplicationViewAssembler.h"
#include "sys/relation-context/domain/service/include/FriendApplicationService.h"
#include "sys/relation-context/domain/service/include/FriendApplicationValidator.h"
#include "sys/relation-context/domain/service/include/FriendShipService.h"
#include "sys/relation-context/domain/service/include/ParticipantService.h"
#include "sys/relation-context/domain/service/include/PrivateChatSessionService.h"

namespace sys::relation::starter
{
    void RelationContextStarter::bootstrap()
    {
        QInjection::addSingleton(new adapter::BackendClientAdapter);
        QInjection::addSingleton(new adapter::NotificationClientAdapter);
        QInjection::addSingleton(new adapter::MessageClientAdapter);

        QInjection::addSingleton(new adapter::FriendApplicationRepositoryAdapter);
        QInjection::addSingleton(new adapter::FriendShipRepositoryAdapter);
        QInjection::addSingleton(new adapter::PrivateChatSessionRepositoryAdapter);
        QInjection::addSingleton(new adapter::ParticipantRepositoryAdapter);

        QInjection::addSingleton(new domain::FriendApplicationValidator);
        QInjection::addSingleton(new domain::ParticipantService);
        QInjection::addSingleton(new domain::PrivateChatSessionService);
        QInjection::addSingleton(new domain::FriendShipService);
        QInjection::addSingleton(new domain::FriendApplicationService);

        QInjection::addSingleton(new application::FriendApplicationViewAssembler);
        QInjection::addSingleton(new application::ChatSessionViewAssembler);

        QInjection::addSingleton(new application::RelationApplicationService);

        QInjection::Pointer<domain::FriendApplicationService> friendApplicationService;
        friendApplicationService->registerAll();
    }
}
