//
// Created by 86150 on 2026/3/22.
//

#include "sys/relation-context/domain/object/include/PrivateChatSession.h"

namespace sys::relation::domain
{
    PrivateChatSession::PrivateChatSession(const QString& privateChatSessionId,
                                           const QString& friendShipId)
        : id(privateChatSessionId),
          friendshipId(friendShipId)
    {
    }

    QSharedPointer<PrivateChatSession> PrivateChatSession::of(const QString& privateChatSessionId,
                                                              const QString& friendShipId)
    {
        return QSharedPointer<PrivateChatSession>(new PrivateChatSession(privateChatSessionId, friendShipId));
    }

    const QString& PrivateChatSession::privateChatSessionId() const
    {
        return id;
    }

    const QString& PrivateChatSession::friendShipId() const
    {
        return friendshipId;
    }

    ChatSessionType PrivateChatSession::getType() const
    {
        return ChatSessionType::PrivateChatSessionType;
    }
}
