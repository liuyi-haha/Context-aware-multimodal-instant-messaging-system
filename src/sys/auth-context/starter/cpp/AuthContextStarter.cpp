#include "../include/AuthContextStarter.h"

#include "dependencypool.h"
#include "sys/auth-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/auth-context/application/service/include/AuthApplicationService.h"
#include "sys/auth-context/domain/service/include/UserCredentialService.h"

namespace sys::auth::starter
{
    void AuthContextStarter::bootstrap()
    {
        QInjection::addSingleton(new adapter::BackendClientAdapter);
        QInjection::addSingleton(new domain::UserCredentialService);
        QInjection::addSingleton(new application::AuthApplicationService);
    }
}
