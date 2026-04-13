#pragma once
#include "sys/auth-context/application/service/include/AuthApplicationService.h"
#include "sys/user-context/port/client/include/AuthClient.h"

namespace sys::user::adapter
{
    class UserAuthClientAdapter final : public port::AuthClient
    {
    public:
        port::AuthResult validateAndHashPassword(const QString& password) override;

    private:
        auth::application::AuthApplicationService* authApplicationService = QInjection::Inject;
    };
}
