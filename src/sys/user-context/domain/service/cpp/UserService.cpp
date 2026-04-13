//
// Created by 86150 on 2026/3/23.
//

#include "../include/UserService.h"

#include <QFileInfo>
#include <QSet>

#include "sys/user-context/domain/exception/InvalidAccountException.h"
#include "sys/user-context/domain/exception/InvalidAvatarException.h"
#include "sys/user-context/domain/exception/InvalidUserIdException.h"
#include "sys/user-context/domain/exception/UserNotFoundException.h"
#include "sys/user-context/domain/exception/UserServiceDependencyException.h"

namespace sys::user::domain
{
    UserService::UserService(
        UserValidator* userValidator,
        port::BackendClient* backendClient,
        port::FileClient* fileClient,
        port::UserRepository* userRepository)
        : userValidator(userValidator),
          backendClient(backendClient),
          fileClient(fileClient),
          userRepository(userRepository)
    {
    }

    void UserService::checkConfig(const bool requireUserValidator, const bool requireFileClient) const
    {
        if (backendClient == nullptr || userRepository == nullptr)
        {
            throw UserServiceDependencyException();
        }

        if (requireUserValidator && userValidator == nullptr)
        {
            throw UserServiceDependencyException();
        }

        if (requireFileClient && fileClient == nullptr)
        {
            throw UserServiceDependencyException();
        }
    }

    QString UserService::registerUser(
        const QString& nickname,
        const QString& phone,
        const QString& password,
        const QFileInfo& avatarFileInfo)
    {
        checkConfig(true, true);

        // 把头像读取到本地缓存
        QFile avatarFile(avatarFileInfo.absoluteFilePath());
        if (!avatarFile.open(QIODevice::ReadOnly))
        {
            throw InvalidAvatarException();
        }
        auto avatar = avatarFile.readAll();
        avatarFile.close();

        QString hashedPassword = userValidator->validate(nickname, phone, password, avatar);

        auto [userId, avatarFileId] = backendClient->registerUser(
            hashedPassword, nickname, phone, avatarFileInfo);


        auto user = User::of(userId, nickname, phone, avatar, avatarFileId);

        fileClient->uploadAvatar(user);
        userRepository->save(user);

        return userId;
    }

    QSharedPointer<domain::User> UserService::searchUser(const QString& account)
    {
        checkConfig(false, false);

        if (!User::checkAccount(account))
        {
            throw InvalidAccountException();
        }

        auto user = backendClient->searchUser(account);
        if (user == nullptr)
        {
            throw domain::UserNotFoundException();
        }
        userRepository->save(user);
        return user;
    }

    QSharedPointer<domain::User> UserService::getUser(const QString& userId)
    {
        checkConfig(false, false);

        if (!User::checkId(userId))
        {
            throw InvalidUserIdException();
        }

        if (auto user = userRepository->of(userId); user != nullptr)
        {
            return user;
        }

        auto user = backendClient->searchUser(userId);
        if (user == nullptr)
        {
            throw domain::UserNotFoundException();
        }
        userRepository->save(user);
        return user;
    }

    QList<QSharedPointer<domain::User>> UserService::getUsers(const QSet<QString>& userIds)
    {
        checkConfig(false, false);

        if (userIds.isEmpty())
        {
            return {};
        }

        QList<QSharedPointer<domain::User>> users = userRepository->ofAll(userIds);
        QSet<QString> foundUserIds;
        for (const auto& user : users)
        {
            if (user != nullptr)
            {
                foundUserIds.insert(user->userId());
            }
        }

        QSet<QString> missingUserIds = userIds;
        for (const auto& foundUserId : foundUserIds)
        {
            missingUserIds.remove(foundUserId);
        }

        for (const auto& missingUserId : missingUserIds)
        {
            if (!User::checkId(missingUserId))
            {
                continue;
            }

            try
            {
                auto remoteUser = backendClient->searchUser(missingUserId);
                if (remoteUser == nullptr)
                {
                    continue;
                }

                userRepository->save(remoteUser);
                users.append(remoteUser);
            }
            catch (const domain::UserNotFoundException&)
            {
                // 对于批量查询，不存在的用户属于可接受场景，直接忽略。
                continue;
            }
        }

        return users;
    }
}
