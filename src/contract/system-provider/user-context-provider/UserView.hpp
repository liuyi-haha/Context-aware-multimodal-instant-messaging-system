//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>
#include <QSharedPointer>

#include "sys/user-context/domain/object/include/User.h"

namespace contract::user
{
    struct UserView
    {
        QString userId;
        QString nickname;
        QString avatarFileId;
        std::optional<QByteArray> avatar; // 可选，可能需要异步加载

        void from(QSharedPointer<sys::user::domain::User> user)
        {
            if (!user)
            {
                return;
            }

            userId = user->userId();
            nickname = user->nicknameValue();
            avatarFileId = user->avatarFileId();
        }
    };
}
