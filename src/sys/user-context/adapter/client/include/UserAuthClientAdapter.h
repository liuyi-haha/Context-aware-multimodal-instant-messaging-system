#pragma once
#include "sys/user-context/port/client/include/AuthClient.h"

namespace sys::user::adapter
{
    class UserAuthClientAdapter final : public port::AuthClient
    {
    public:
        port::AuthResult validateAndHashPassword(const QString& password) override;
    };
}