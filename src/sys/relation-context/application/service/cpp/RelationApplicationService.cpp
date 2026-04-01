//
// Created by 86150 on 2026/3/25.
//

#include "../include/RelationApplicationService.h"

#include <exception>

#include "sys/common/exception/InfraExcception.h"
#include "sys/relation-context/application/assembler/include/FriendApplicationViewAssembler.h"

namespace sys::relation::application
{
    RelationApplicationService::RelationApplicationService(
        domain::FriendApplicationService* friendApplicationService,
        FriendApplicationViewAssembler* friendApplicationViewAssembler,
        domain::PrivateChatSessionService* privateChatSessionService,
        ChatSessionViewAssembler* chatSessionViewAssembler,
        QObject* parent)
        : QObject(parent),
          friendApplicationService(friendApplicationService),
          friendApplicationViewAssembler(friendApplicationViewAssembler),
          privateChatSessionService(privateChatSessionService),
          chatSessionViewAssembler(chatSessionViewAssembler)
    {
    }

    contract::relation::SendFriendApplicationResponse RelationApplicationService::sendFriendApplication(
        const contract::relation::SendFriendApplicationRequest& request)
    {
        contract::relation::SendFriendApplicationResponse response{};
        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            response.friendApplicationId = friendApplicationService->sendFriendApplication(
                request.targetUserId,
                request.verificationMessage,
                request.recipientRemark);
            response.success = true;
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }

        return response;
    }

    contract::relation::AcceptFriendApplicationResponse RelationApplicationService::acceptFriendApplication(
        const contract::relation::AcceptFriendApplicationRequest& request)
    {
        contract::relation::AcceptFriendApplicationResponse response{};
        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            friendApplicationService->acceptFriendApplication(request.applicationId, request.recipientRemark);
            response.success = true;
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }

        return response;
    }

    contract::relation::RejectFriendApplicationResponse RelationApplicationService::rejectFriendApplication(
        const contract::relation::RejectFriendApplicationRequest& request)
    {
        contract::relation::RejectFriendApplicationResponse response{};
        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            friendApplicationService->rejectFriendApplication(request.friendApplicationId);
            response.success = true;
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }

        return response;
    }

    QList<contract::relation::FriendApplicationView> RelationApplicationService::getFriendApplications()
    {
        checkConfig();

        try
        {
            const auto applications = friendApplicationService->getFriendApplications();
            return friendApplicationViewAssembler->toFriendApplicationViews(applications);
        }
        catch (const std::exception& ex)
        {
            return {};
        }
    }

    QList<contract::relation::ChatSessionView> RelationApplicationService::getChatSessions()
    {
        checkConfig();
        try
        {
            QList<QSharedPointer<domain::PrivateChatSession>> privateChatSessions = privateChatSessionService->
                getPrivateChatSessions();
            return chatSessionViewAssembler->toChatSessionViewsByLastMessageSendTimeDesc(privateChatSessions);
        }
        catch (const std::exception& ex)
        {
            return {};
        }
    }

    bool RelationApplicationService::checkConfigAndSetResponse(contract::BaseResponse& response)
    {
        if (friendApplicationService == nullptr || friendApplicationViewAssembler
            == nullptr || privateChatSessionService == nullptr || chatSessionViewAssembler == nullptr)
        {
            response.success = false;
            response.errMsg = QStringLiteral("RelationApplicationService is not properly configured.");
            return false;
        }

        return true;
    }

    void RelationApplicationService::checkConfig() const
    {
        if (friendApplicationService == nullptr || friendApplicationViewAssembler == nullptr ||
            privateChatSessionService == nullptr || chatSessionViewAssembler == nullptr)
        {
            throw core::InfraException("RelationApplicationService is not properly configured.");
        }
    }
}
