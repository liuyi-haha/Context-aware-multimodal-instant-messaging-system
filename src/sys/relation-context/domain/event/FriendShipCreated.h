//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include <QString>

#include "event/DomainEvent.h"

// 已废弃
namespace sys::relation::domain
{
    class FriendShipCreated : public core::DomainEvent
    {
    public:
        FriendShipCreated(const QString& friendShipId, const QString& privateChatSessionId, const QString& targetUserId)
            : friendShipId(friendShipId),
              privateChatSessionId(privateChatSessionId),
              targetUserId(targetUserId)
        {
        }

        const QString& friendShip() const { return friendShipId; }
        const QString& privateChatSession() const { return privateChatSessionId; }
        const QString& targetUser() const { return targetUserId; }

    private:
        QString friendShipId;
        QString ownerUserId;
        QString peerUserId;
        QString privateChatSessionId;
        QString ownerParticipantId;
        QString peerParticipantId;
    };

    class FriendShipCreatedSubscriber
    {
    public:
        virtual ~FriendShipCreatedSubscriber() = default;
        virtual void handle(FriendShipCreated) = 0;
    };
}
