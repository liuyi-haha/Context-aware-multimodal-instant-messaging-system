#pragma once

#include "dependencyinjector.h"
#include "sys/common/port/UserClient.h"
#include "sys/user-context/application/service/include/UserApplicationService.h"

namespace sys::common::adapter
{
    class UserClientAdapter final : public port::UserClient
    {
    public:
        explicit UserClientAdapter(
            sys::user::application::UserApplicationService* userApplicationService = QInjection::Inject);
        ~UserClientAdapter() override = default;

        port::UserInfo getUser(const QString& userId) override;
        QHash<QString, port::UserInfo> getUsers(const QSet<QString>& userIds) override;

    private:
        sys::user::application::UserApplicationService* userApplicationService = nullptr;
        void checkConfig();
    };
}
