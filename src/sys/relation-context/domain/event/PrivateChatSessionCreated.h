//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include <QString>

#include "event/DomainEvent.h"

// 已废弃
namespace sys::relation::domain
{
    class PrivateChatSessionCreated : public core::DomainEvent
    {
    public:
        PrivateChatSessionCreated(const QString& privateChatSessionId,
                                  const QString& friendShipId,
                                  const QString& targetUserId)
            : privateChatSessionId(privateChatSessionId),
              friendShipId(friendShipId),
              targetUserId(targetUserId)
        {
        }

        const QString& privateChatSession() const { return privateChatSessionId; }
        const QString& friendShip() const { return friendShipId; }
        const QString& targetUser() const { return targetUserId; }

    private:
        QString privateChatSessionId;
        QString friendShipId;
        QString targetUserId;
    };

    class PrivateChatSessionCreatedSubscriber
    {
    public:
        virtual ~PrivateChatSessionCreatedSubscriber() = default;
        virtual void handle(PrivateChatSessionCreated) = 0;
    };
}
