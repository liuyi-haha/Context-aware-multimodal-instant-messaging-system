//
// Created by 86150 on 2026/3/25.
//

#include "../include/FriendApplicationService.h"

#include "dependencypointer.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/exception/FriendApplicationServiceDependencyException.h"
#include "sys/relation-context/domain/service/include/FriendShipService.h"
#include "sys/relation-context/domain/service/include/ParticipantService.h"
#include "sys/relation-context/domain/service/include/PrivateChatSessionService.h"

namespace sys::relation::domain
{
    FriendApplicationService::FriendApplicationService(
        port::BackendClient* backendClient,
        FriendApplicationValidator* friendApplicationValidator,
        port::FriendApplicationRepository* friendApplicationRepository,
        port::FriendShipRepository* friendShipRepository,
        QObject* parent)
        : QObject(parent),
          backendClient(backendClient),
          friendApplicationValidator(friendApplicationValidator),
          friendApplicationRepository(friendApplicationRepository),
          friendShipRepository(friendShipRepository)
    {
    }

    void FriendApplicationService::checkConfig() const
    {
        if (backendClient == nullptr
            || friendApplicationValidator == nullptr
            || friendApplicationRepository == nullptr
            || friendShipRepository == nullptr
        )
        {
            throw FriendApplicationServiceDependencyException();
        }
    }

    QString FriendApplicationService::sendFriendApplication(const QString& targetUserId,
                                                            const QString& verificationMessage,
                                                            const QString& recipientRemark)
    {
        checkConfig();

        const QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();

        friendApplicationValidator->validateSendFriendApplicationInputs(verificationMessage, recipientRemark,
                                                                        targetUserId);


        const auto backendResult = backendClient->sendFriendApplication(
            targetUserId,
            verificationMessage,
            recipientRemark);

        auto friendApplication = FriendApplication::of(
            currentUserId,
            backendResult.friendApplicationId,
            targetUserId,
            verificationMessage,
            recipientRemark,
            backendResult.applyTime);
        friendApplicationRepository->save(friendApplication);

        return backendResult.friendApplicationId;
    }

    void FriendApplicationService::acceptFriendApplication(const QString& friendApplicationId, const QString& remark)
    {
        checkConfig();

        const QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();
        auto friendApplication = friendApplicationValidator->validateHandleFriendApplicationInputs(
            friendApplicationId,
            currentUserId);

        const auto backendResult = backendClient->acceptFriendApplication(friendApplicationId, remark);

        friendApplication->accept();
        friendApplicationRepository->save(friendApplication);

        if (!backendResult.isFriendShipCreated || !backendResult.newFriendShipInfo.has_value())
        {
            return;
        }

        const auto& info = backendResult.newFriendShipInfo.value();
        FriendApplicationAccepted event(
            friendApplicationId,
            friendApplication->applicantUserId(),
            friendApplication->targetUserId(),
            info.friendShipId,
            remark,
            info.privateChatSessionId,
            info.applicantParticipantId,
            info.targetUserParticipantId);
        publish(event);
    }

    void FriendApplicationService::rejectFriendApplication(const QString& friendApplicationId)
    {
        checkConfig();

        const QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();
        auto friendApplication = friendApplicationValidator->validateHandleFriendApplicationInputs(
            friendApplicationId,
            currentUserId);

        backendClient->rejectFriendApplication(friendApplicationId);

        friendApplication->reject();
        friendApplicationRepository->save(friendApplication);
    }

    QList<QSharedPointer<FriendApplication>> FriendApplicationService::getFriendApplications()
    {
        checkConfig();

        const QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();
        auto asApplicantFriendApplications = friendApplicationRepository->ofApplicantUserId(
            currentUserId);
        QList<QSharedPointer<FriendApplication>> asTargetFriendApplications = friendApplicationRepository->
            ofTargetUserId(currentUserId);

        return FriendApplication::composeAndSortByApplyTimeDesc(asApplicantFriendApplications,
                                                                asTargetFriendApplications);
    }

    void FriendApplicationService::publish(const FriendApplicationAccepted& friendApplicationAccepted)
    {
        for (auto subscriber : friendApplicationAcceptedSubscribers)
        {
            subscriber->handle(friendApplicationAccepted);
        }
    }

    void FriendApplicationService::registerAll()
    {
        QInjection::Pointer<FriendShipService> friendShipService;
        QInjection::Pointer<PrivateChatSessionService> privateChatSessionService;
        QInjection::Pointer<ParticipantService> participantService;

        friendApplicationAcceptedSubscribers.push_back(friendShipService.data());
        friendApplicationAcceptedSubscribers.push_back(privateChatSessionService.data());
        friendApplicationAcceptedSubscribers.push_back(participantService.data());
    }

    void FriendApplicationService::registerFriendApplicationAcceptedEventSubscriber(
        FriendApplicationAcceptedEventSubscriber* subscriber)
    {
        friendApplicationAcceptedSubscribers.push_back(subscriber);
    }
}
