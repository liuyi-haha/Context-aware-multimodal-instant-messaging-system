//
// Created by 86150 on 2026/3/22.
//

#include "sys/relation-context/domain/object/include/FriendShip.h"

namespace sys::relation::domain
{
    FriendShip::FriendShip(const QString& friendShipId,
                           const QString& userId,
                           const QString& friendUserId)
        : friendshipId(friendShipId),
          userId(userId),
          peerId(friendUserId)
    {
    }

    QSharedPointer<sys::relation::domain::FriendShip> FriendShip::of(
        const QString& friendShipId, const QString& userId,
        const QString& friendUserId)
    {
        return QSharedPointer<FriendShip>(new FriendShip(friendShipId, userId, friendUserId));
    }

    const QString& FriendShip::friendShipId() const
    {
        return friendshipId;
    }

    const QString& FriendShip::ownerUserId() const
    {
        return userId;
    }

    const QString& FriendShip::peerUserId() const
    {
        return peerId;
    }
}
