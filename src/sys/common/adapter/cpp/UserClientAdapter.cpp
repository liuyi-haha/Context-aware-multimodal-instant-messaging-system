#include "../include/UserClientAdapter.h"

#include <QSet>

#include "sys/common/exception/InfraExcception.h"

namespace sys::common::adapter
{
    UserClientAdapter::UserClientAdapter(
        sys::user::application::UserApplicationService* userApplicationService)
        : userApplicationService(userApplicationService)
    {
    }

    port::UserInfo UserClientAdapter::getUser(const QString& userId)
    {
        checkConfig();
        auto user = userApplicationService->getUser({userId});
        return port::UserInfo{
            .userId = user.userView.userId,
            .nickname = user.userView.nickname,
            .avatarFileId = user.userView.avatarFileId
        };
    }

    QHash<QString, port::UserInfo> UserClientAdapter::getUsers(const QSet<QString>& userIds)
    {
        checkConfig();
        QHash<QString, port::UserInfo> result;
        const auto& users = userApplicationService->getUserInfos(userIds);
        for (auto it = users.constBegin(); it != users.constEnd(); ++it)
        {
            const auto& userId = it.key();
            port::UserInfo userInfo{
                .userId = it.value().userId,
                .nickname = it.value().nickname,
                .avatarFileId = it.value().avatarFileId
            };
            result.insert(userId, userInfo);
        }

        return result;
    }

    void UserClientAdapter::checkConfig()
    {
        if (userApplicationService == nullptr)
        {
            throw core::InfraException("UserClientAdapter依赖的UserApplicationService未配置");
        }
    }
}

