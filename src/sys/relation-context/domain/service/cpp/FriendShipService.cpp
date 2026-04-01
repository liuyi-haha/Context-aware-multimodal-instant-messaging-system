//
// Created by 86150 on 2026/3/26.
//

#include "../include/FriendShipService.h"

#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"
#include "sys/relation-context/domain/object/include/FriendShip.h"

namespace sys::relation::domain
{
    FriendShipService::FriendShipService(
        port::FriendShipRepository* friendShipRepository) :
        friendShipRepository(friendShipRepository)
    {
    }

    void FriendShipService::handle(FriendApplicationAccepted friendApplicationAccepted)
    {
        checkConfig();

        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        QString peerUserId = friendApplicationAccepted.applicantUserId == currentUserId
                                 ? friendApplicationAccepted.targetUserId
                                 : friendApplicationAccepted.applicantUserId;
        auto friendShip = FriendShip::of(friendApplicationAccepted.friendShipId,
                                         currentUserId,
                                         peerUserId);
        friendShipRepository->save(friendShip);
    }

    void FriendShipService::checkConfig()
    {
        if (friendShipRepository == nullptr)
        {
            throw core::InfraException("FriendShipService not configured");
        }
    }
}
