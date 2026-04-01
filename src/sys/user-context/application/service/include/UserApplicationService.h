//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QObject>

#include "dependencyinjector.h"
#include "contract/system-provider/user-context-provider/Register.hpp"
#include "contract/system-provider/user-context-provider/SearchUser.h"
#include "contract/system-provider/user-context-provider/GetUser.h"
#include "sys/user-context/domain/service/include/UserService.h"

namespace sys::user::application
{
    class UserApplicationService : public QObject
    {
        Q_OBJECT

    public:
        explicit UserApplicationService(domain::UserService* userService = QInjection::Inject,
                                        QObject* parent = nullptr);
        contract::user::RegisterResponse registerUser(const contract::user::RegisterRequest& request);
        contract::user::SearchUserResponse searchUser(const contract::user::SearchUserRequest& request);
        contract::user::GetUserResponse getUser(const contract::user::GetUserRequest& request);
        // 来自其它上下文的调用

        struct UserInfo
        {
            QString userId;
            QString nickname;
            QString avatarFileId;

            bool operator==(const UserInfo& other) const
            {
                return userId == other.userId && nickname == other.nickname && avatarFileId == other.avatarFileId;
            }
        };


        QHash<QString, UserInfo> getUserInfos(const QSet<QString>& userIds);

    private:
        domain::UserService* userService = nullptr;

    private:
        bool checkConfigAndSetResponse(contract::BaseResponse& response);
        bool checkConfig();
    };
}
