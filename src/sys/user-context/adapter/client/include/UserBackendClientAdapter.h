#pragma once

#include "OAIUserHttpFileElement.h"
#include "OAIUserRegisterUser_200_response.h"
#include "OAIUserSearchUser_200_response.h"
#include "sys/user-context/port/client/include/BackendClient.h"

namespace sys::user::adapter
{
    class IUserServiceApiGateway
    {
    public:
        virtual ~IUserServiceApiGateway() = default;
        virtual OpenAPIUser::OAIUserRegisterUser_200_response registerUser(
            const QString& nickname, const QString& phone, const OpenAPIUser::OAIUserHttpFileElement& avatar,
            const QString& password) = 0;
        virtual OpenAPIUser::OAIUserSearchUser_200_response searchUser(const QString& keyword) = 0;
    };

    class UserServerApiGateway : public IUserServiceApiGateway
    {
    public:
        OpenAPIUser::OAIUserRegisterUser_200_response
        registerUser(const QString& nickname, const QString& phone,
                     const OpenAPIUser::OAIUserHttpFileElement& avatar, const QString& password) override;

        OpenAPIUser::OAIUserSearchUser_200_response searchUser(const QString& keyword) override;
    };

    class BackendClientAdapter final : public port::BackendClient
    {
    public:
        enum class RegisterUserErrCode
        {
            None,
            NicknameInvalid,
            PhoneRegistered,
            UnknownError
        };

        enum class SearchUserErrCode
        {
            None,
            UserNotFound,
            KeywordInvalid,
            UnknownError
        };

    public:
        explicit BackendClientAdapter(IUserServiceApiGateway* apiGateway = new UserServerApiGateway())
            : apiGateway(apiGateway)
        {
        }

        RegisterUserResult registerUser(const QString& hashedPassword, const QString& nickname,
                                        const QString& phone, const QFileInfo& avatarFileInfo) override;
        QSharedPointer<domain::User> searchUser(const QString& account) override;

    private:
        static RegisterUserErrCode fromErrCode(const QString& errCode);
        static QString toErrCode(RegisterUserErrCode errCode);
        static SearchUserErrCode fromSearchErrCode(const QString& errCode);

    private:
        IUserServiceApiGateway* apiGateway;
    };
}
