#pragma once

#include "OAIChatAcceptFriendApplication_200_response.h"
#include "OAIChatAcceptFriendApplication_request.h"
#include "OAIChatRejectFriendApplication_200_response.h"
#include "OAIChatSendFriendApplication_200_response.h"
#include "OAIChatAcceptFriendApplication_request.h"
#include "OAIChatSendFriendApplication_request.h"
#include "sys/relation-context/port/client/include/BackendClient.h"

namespace sys::relation::adapter
{
    class IChatApiGateway
    {
    public:
        virtual ~IChatApiGateway() = default;
        virtual OpenAPIChat::OAIChatSendFriendApplication_200_response sendFriendApplication(
            const OpenAPIChat::OAIChatSendFriendApplication_request& request) = 0;
        virtual OpenAPIChat::OAIChatAcceptFriendApplication_200_response acceptFriendApplication(
            const OpenAPIChat::OAIChatAcceptFriendApplication_request& request, const QString& friendApplicationId) = 0;
        virtual OpenAPIChat::OAIChatRejectFriendApplication_200_response rejectFriendApplication(
            const QString& friendApplicationId) = 0;
    };

    class ChatServerApiGateway : public IChatApiGateway
    {
    public:
        OpenAPIChat::OAIChatSendFriendApplication_200_response sendFriendApplication(
            const OpenAPIChat::OAIChatSendFriendApplication_request& request) override;
        OpenAPIChat::OAIChatAcceptFriendApplication_200_response acceptFriendApplication(
            const OpenAPIChat::OAIChatAcceptFriendApplication_request& request,
            const QString& friendApplicationId) override;
        OpenAPIChat::OAIChatRejectFriendApplication_200_response rejectFriendApplication(
            const QString& friendApplicationId) override;
    };

    class BackendClientAdapter final : public port::BackendClient
    {
    public:
        enum class SendFriendApplicationErrCode
        {
            None,
            UserNotFound,
            DuplicateApplication,
            AlreadyFriend,
            SelfFriend,
            UnknownError
        };

        enum class HandleFriendApplicationErrCode
        {
            None,
            ApplicationNotFound,
            ApplicationAlreadyHandled,
            UnknownError
        };

    public:
        explicit BackendClientAdapter(IChatApiGateway* chatApiGateway = new ChatServerApiGateway())
            : chatApiGateway(chatApiGateway)
        {
        }

        port::SendFriendApplicationResult sendFriendApplication(const QString& targetUserId,
                                                                const QString& verificationMessage,
                                                                const QString& recipientRemark) override;
        port::AcceptFriendApplicationResult acceptFriendApplication(const QString& friendApplicationId,
                                                                    const QString& remark) override;
        void rejectFriendApplication(const QString& friendApplicationId) override;

    private:
        static SendFriendApplicationErrCode fromSendErrCode(const QString& errCode);
        static HandleFriendApplicationErrCode fromHandleErrCode(const QString& errCode);

    private:
        IChatApiGateway* chatApiGateway = nullptr;
    };
}
