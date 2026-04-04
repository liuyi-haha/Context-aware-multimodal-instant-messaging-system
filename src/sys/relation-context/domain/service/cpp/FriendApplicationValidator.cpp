#include "../include/FriendApplicationValidator.h"

#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/exception/FriendApplicationAlreadyProcessedException.h"
#include "sys/relation-context/domain/exception/FriendApplicationHandleUnauthorizedException.h"
#include "sys/relation-context/domain/exception/FriendApplicationNotFoundException.h"
#include "sys/relation-context/domain/exception/FriendRelationAlreadyExistsException.h"
#include "sys/relation-context/domain/exception/InvalidRecipientRemarkException.h"
#include "sys/relation-context/domain/exception/InvalidTargetUserIdException.h"
#include "sys/relation-context/domain/exception/InvalidVerificationMessageException.h"
#include "sys/relation-context/domain/exception/PendingFriendApplicationAlreadyExistsException.h"
#include "sys/relation-context/domain/object/include/FriendApplication.h"

namespace sys::relation::domain
{
    FriendApplicationValidator::FriendApplicationValidator(
        port::FriendApplicationRepository* friendApplicationRepository,
        port::FriendShipRepository* friendShipRepository,
        QObject* parent)
        : QObject(parent),
          friendApplicationRepository(friendApplicationRepository),
          friendShipRepository(friendShipRepository)
    {
    }

    void FriendApplicationValidator::validateSendFriendApplicationInputs(const QString& verificationMessage,
                                                                         const QString& recipientRemark,
                                                                         const QString& targetUserId) const
    {
        QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        if (!VerificationMessage::checkVerificationMessage(verificationMessage))
        {
            throw InvalidVerificationMessageException();
        }

        if (!core::RecipientRemark::checkRecipientRemark(recipientRemark))
        {
            throw InvalidRecipientRemarkException();
        }

        if (currentUserId == targetUserId)
        {
            throw ApplyFriendToMyselfException();
        }
        validatePendingApplicationExistence(currentUserId, targetUserId);
        validateFriendShipExistence(currentUserId, targetUserId);
    }

    QSharedPointer<FriendApplication> FriendApplicationValidator::validateHandleFriendApplicationInputs(
        const QString& friendApplicationId,
        const QString& currentUserId) const
    {
        auto friendApplication = validateExistence(friendApplicationId);
        validateIsProcessed(friendApplication);

        // 只有目标用户才能处理该好友申请。
        if (friendApplication->targetUserId() != currentUserId)
        {
            throw FriendApplicationHandleUnauthorizedException();
        }

        return friendApplication;
    }


    void FriendApplicationValidator::validateIsProcessed(
        const QSharedPointer<FriendApplication>& friendApplication) const
    {
        if (friendApplication == nullptr)
        {
            throw FriendApplicationNotFoundException();
        }

        if (friendApplication->applicationStatus() != ApplicationStatus::Pending)
        {
            throw FriendApplicationAlreadyProcessedException();
        }
    }

    QSharedPointer<FriendApplication> FriendApplicationValidator::validateExistence(
        const QString& friendApplicationId) const
    {
        auto friendApplication = friendApplicationRepository->of(friendApplicationId);
        if (!friendApplication)
        {
            throw FriendApplicationNotFoundException();
        }
        return friendApplication;
    }

    void FriendApplicationValidator::validatePendingApplicationExistence(const QString& currentUserId,
                                                                         const QString& targetUserId) const
    {
        if (friendApplicationRepository->of(currentUserId, targetUserId, ApplicationStatus::Pending) != nullptr)
        {
            throw PendingFriendApplicationAlreadyExistsException();
        }
    }

    void FriendApplicationValidator::validateFriendShipExistence(const QString& currentUserId,
                                                                 const QString& targetUserId) const
    {
        if (friendShipRepository->of(currentUserId, targetUserId) != nullptr)
        {
            throw FriendShipAlreadyExistsException();
        }
    }
}
