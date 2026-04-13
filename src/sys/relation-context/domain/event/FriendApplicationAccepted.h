//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include <QString>

#include "sys/common/event/DomainEvent.h"

namespace sys::relation::domain
{
    struct FriendApplicationAccepted : public core::DomainEvent
    {
        FriendApplicationAccepted(const QString& friendApplicationId, const QString& applicantUserId,
                                  const QString& targetUserId, const QString& friendShipId,
                                  const QString& recipientRemark, const QString& privateChatSessionId,
                                  const QString& applicantParticipantId, const QString& targetUserParticipantId)
            : friendApplicationId(friendApplicationId),
              applicantUserId(applicantUserId),
              targetUserId(targetUserId),
              friendShipId(friendShipId),
              remark(recipientRemark),
              privateChatSessionId(privateChatSessionId),
              applicantParticipantId(applicantParticipantId),
              targetUserParticipantId(targetUserParticipantId)
        {
        }

        const QString friendApplicationId;
        const QString applicantUserId;
        const QString targetUserId;
        const QString friendShipId;
        const QString remark; // 请记住，这是我给对方的备注，对方可能是申请人，也是申请目标
        const QString privateChatSessionId;
        const QString applicantParticipantId;
        const QString targetUserParticipantId;
    };


    class FriendApplicationAcceptedEventSubscriber
    {
    public:
        virtual ~FriendApplicationAcceptedEventSubscriber() = default;
        virtual void handle(FriendApplicationAccepted) = 0;
    };
}
