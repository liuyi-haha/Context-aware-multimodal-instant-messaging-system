#include "../include/UserContextStarter.h"

#include "dependencypool.h"

#include "sys/user-context/adapter/client/include/UserAuthClientAdapter.h"
#include "sys/user-context/adapter/client/include/UserBackendClientAdapter.h"
#include "sys/user-context/adapter/client/include/UserFileClientAdapter.h"
#include "sys/user-context/adapter/repository/include/UserRepositoryAdapter.h"
#include "sys/user-context/application/service/include/UserApplicationService.h"
#include "sys/user-context/domain/service/include/UserService.h"
#include "sys/user-context/domain/service/include/UserValidator.h"

namespace sys::user::starter
{
    void UserContextStarter::bootstrap()
    {
        QInjection::addSingleton(new adapter::UserAuthClientAdapter);
        QInjection::addSingleton(new adapter::BackendClientAdapter);
        QInjection::addSingleton(new adapter::FileClientAdapter);
        QInjection::addSingleton(new adapter::UserRepositoryAdapter);
        QInjection::addSingleton(new domain::UserValidator);
        QInjection::addSingleton(new domain::UserService);
        QInjection::addSingleton(new application::UserApplicationService);
    }
}
