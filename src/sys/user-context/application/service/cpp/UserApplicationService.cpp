//
// Created by 86150 on 2026/3/23.
//

#include "../include/UserApplicationService.h"
#include "sys/user-context/domain/exception/UserNotFoundException.h"

#include <exception>
#include <QSet>
#include <QDebug>

#include "sys/common/exception/InfraExcception.h"

namespace sys::user::application
{
    UserApplicationService::UserApplicationService(domain::UserService* userService, QObject* parent)
        : QObject(parent),
          userService(userService)
    {
    }

    bool UserApplicationService::checkConfig()
    {
        if (userService == nullptr)
        {
            throw core::InfraException("UserApplicationService is not configured.");
        }
        return true;
    }

    QHash<QString, UserApplicationService::UserInfo> UserApplicationService::getUserInfos(const QSet<QString>& userIds)
    {
        checkConfig();

        try
        {
            const auto users = userService->getUsers(userIds);
            QHash<QString, UserInfo> infos;
            for (const auto& user : users)
            {
                if (user == nullptr)
                {
                    continue;
                }

                const QString userId = user->userId();
                infos[userId] = UserInfo{userId, user->nicknameValue(), user->avatarFileId()};
            }
            return infos;
        }
        catch (const std::exception& ex)
        {
            qDebug() << QString::fromLocal8Bit(ex.what());
            return {};
        }
    }

    bool UserApplicationService::checkConfigAndSetResponse(contract::BaseResponse& response)
    {
        if (userService == nullptr)
        {
            response.success = false;
            response.errMsg = QStringLiteral("UserService is not configured.");
            return false;
        }
        return true;
    }

    contract::user::RegisterResponse UserApplicationService::registerUser(
        const contract::user::RegisterRequest& request)
    {
        contract::user::RegisterResponse response{};

        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            response.userId = userService->registerUser(request.nickname, request.phone, request.password,
                                                        request.avatar);
            response.success = true;
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }

        return response;
    }

    contract::user::SearchUserResponse UserApplicationService::searchUser(
        const contract::user::SearchUserRequest& request)
    {
        contract::user::SearchUserResponse response{};

        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            const auto user = userService->searchUser(request.keyword);
            contract::user::UserView view;
            view.from(user);
            response.userView = view;
            response.success = true;
        }
        catch (const domain::UserNotFoundException& ex)
        {
            response.success = true;
            response.userView.reset();
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }

        return response;
    }

    contract::user::GetUserResponse UserApplicationService::getUser(const contract::user::GetUserRequest& request)
    {
        contract::user::GetUserResponse response{};

        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            const auto user = userService->getUser(request.userId);
            response.userView.from(user);
            response.success = true;
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }

        return response;
    }
}
