#include "../include/UserClientAdapter.h"

#include <QSet>

namespace sys::common::adapter
{
    UserClientAdapter::UserClientAdapter(
        sys::user::application::UserApplicationService* userApplicationService)
        : userApplicationService(userApplicationService)
    {
    }

    port::UserInfo UserClientAdapter::getUser(const QString& userId)
    {
        auto user = userApplicationService->getUser({userId});
        return port::UserInfo{
            .userId = user.userView.userId,
            .nickname = user.userView.nickname,
            .avatarFileId = user.userView.avatarFileId
        };
    }

    QHash<QString, port::UserInfo> UserClientAdapter::getUsers(const QSet<QString>& userIds)
    {
        QHash<QString, port::UserInfo> users;
        if (userApplicationService == nullptr)
        {
            return users;
        }

        QSet<QString> uniqueUserIds;
        for (const auto& userId : userIds)
        {
            const QString normalizedUserId = userId.trimmed();
            if (!normalizedUserId.isEmpty())
            {
                uniqueUserIds.insert(normalizedUserId);
            }
        }

        for (const auto& userId : uniqueUserIds)
        {
            contract::user::GetUserRequest request;
            request.userId = userId;

            const auto response = userApplicationService->getUser(request);
            if (!response.success)
            {
                continue;
            }

            port::UserInfo userInfo;
            userInfo.userId = response.userView.userId.trimmed();
            userInfo.nickname = response.userView.nickname;
            userInfo.avatarFileId = response.userView.avatarFileId;

            if (userInfo.userId.isEmpty())
            {
                userInfo.userId = userId;
            }

            users.insert(userId, userInfo);
        }

        return users;
    }
}

