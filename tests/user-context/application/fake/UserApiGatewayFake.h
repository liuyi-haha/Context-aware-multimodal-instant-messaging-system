//
// Created by 86150 on 2026/3/28.
//

#pragma once

#include <QVector>
#include <optional>

#include "OAIUserSearchUser_200_response.h"
#include "sys/user-context/adapter/client/include/UserBackendClientAdapter.h"

namespace tests::user::fake
{
    class UserServiceApiGatewayFake : public sys::user::adapter::IUserServiceApiGateway
    {
    public:
        struct FakeUserProfile
        {
            QString userId;
            QString nickname;
            QString avatarFileId;
            QString phone;
            QString description;
        };

        static FakeUserProfile buildProfile(const QString& userId,
                                            const QString& phone,
                                            const QString& nickname = QStringLiteral("测试用户"),
                                            const QString& avatarFileId = QStringLiteral("avatar-default"),
                                            const QString& description = QString())
        {
            return {userId, nickname, avatarFileId, phone, description};
        }

        OpenAPIUser::OAIUserRegisterUser_200_response
        registerUser(const OpenAPIUser::OAIUserRegisterUser_request& request) override
        {
            ++registerUserCallCount;

            OpenAPIUser::OAIUserRegisterUser_200_response response;
            if (findUserIndexByPhone(request.getPhone()).has_value())
            {
                response.setSuccess(false);
                response.setErrCode(QStringLiteral("PHONE_REGISTERED"));
                response.setErrMsg(QStringLiteral("手机号已被注册"));
                return response;
            }

            FakeUserProfile user;
            user.userId = QString::number(nextGeneratedUserId++);
            user.nickname = request.getNickname();
            user.avatarFileId = QString();
            user.phone = request.getPhone();
            user.description = QString();

            upsertUser(user);

            OpenAPIUser::OAIUserRegisterUser_200_response_data data;
            bool ok = false;
            const int generatedUserId = user.userId.toInt(&ok);
            data.setUserId(ok ? generatedUserId : 0);
            response.setSuccess(true);
            response.setData(data);
            response.setErrMsg(QString());
            return response;
        }

        OpenAPIUser::OAIUserSearchUser_200_response searchUser(const QString& keyword) override
        {
            ++searchUserCallCount;
            lastSearchKeyword = keyword;

            OpenAPIUser::OAIUserSearchUser_200_response response;
            std::optional<int> userIndex = findUserIndexById(keyword);
            if (!userIndex.has_value())
            {
                userIndex = findUserIndexByPhone(keyword);
            }
            if (!userIndex.has_value())
            {
                response.setSuccess(false);
                response.setErrCode(QStringLiteral("USER_NOT_FOUND"));
                response.setErrMsg(QStringLiteral("用户不存在"));
                return response;
            }

            response.setSuccess(true);
            response.setData(toApiData(users[userIndex.value()]));
            return response;
        }

    public:
        int registerUserCallCount = 0;
        int searchUserCallCount = 0;
        QString lastSearchKeyword;

        void setRemoteUser(const QString& userId,
                           const QString& nickname,
                           const QString& avatarFileId,
                           const QString& phone,
                           const QString& description = "")
        {
            upsertRemoteUser(buildProfile(userId, phone, nickname, avatarFileId, description));
        }

        void upsertRemoteUser(const FakeUserProfile& user)
        {
            upsertUser(user);
        }

    private:
        static OpenAPIUser::OAIUserSearchUser_200_response_data toApiData(const FakeUserProfile& profile)
        {
            OpenAPIUser::OAIUserSearchUser_200_response_data data;
            data.setUserId(profile.userId);
            data.setNickname(profile.nickname);
            data.setAvatarFileId(profile.avatarFileId);
            data.setPhone(profile.phone);
            data.setDescription(profile.description);
            return data;
        }

        [[nodiscard]] std::optional<int> findUserIndexById(const QString& userId) const
        {
            for (int i = 0; i < users.size(); ++i)
            {
                if (users[i].userId == userId)
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        [[nodiscard]] std::optional<int> findUserIndexByPhone(const QString& phone) const
        {
            for (int i = 0; i < users.size(); ++i)
            {
                if (users[i].phone == phone)
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        void upsertUser(const FakeUserProfile& user)
        {
            if (const auto byId = findUserIndexById(user.userId); byId.has_value())
            {
                users[byId.value()] = user;
                return;
            }

            if (const auto byPhone = findUserIndexByPhone(user.phone); byPhone.has_value())
            {
                users[byPhone.value()] = user;
                return;
            }

            users.push_back(user);
        }

    private:
        QVector<FakeUserProfile> users;
        qint64 nextGeneratedUserId = 123456789;
    };
}
