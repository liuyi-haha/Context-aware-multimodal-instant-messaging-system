//
// Created by 86150 on 2026/3/26.
//

#include "../include/PrivateChatSessionService.h"

#include <QSet>

#include "sys/relation-context/domain/object/include/PrivateChatSession.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"

namespace sys::relation::domain
{
    PrivateChatSessionService::PrivateChatSessionService(
        port::PrivateChatSessionRepository* privateChatSessionRepository,
        port::ParticipantRepository* participantRepository,
        QObject* parent)
        : QObject(parent),
          privateChatSessionRepository(privateChatSessionRepository),
          participantRepository(participantRepository)
    {
    }

    void PrivateChatSessionService::handle(FriendApplicationAccepted friendApplicationAccepted)
    {
        checkConfig();

        auto session = PrivateChatSession::of(friendApplicationAccepted.privateChatSessionId,
                                              friendApplicationAccepted.friendShipId);
        privateChatSessionRepository->save(session);
    }

    QList<QSharedPointer<PrivateChatSession>> PrivateChatSessionService::getPrivateChatSessions()
    {
        checkConfig();
        // 获取当前用户ID
        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        if (currentUserId.isEmpty())
        {
            return {};
        }

        // 获取当前用户的所有单聊会话参与者
        const auto participants = participantRepository->ofAll(currentUserId, ParticipantRole::PrivateMember);
        if (participants.isEmpty())
        {
            return {};
        }

        // 收集所有单聊会话ID
        QSet<QString> sessionIds;
        for (const auto& participant : participants)
        {
            if (participant)
            {
                sessionIds.insert(participant->sessionId());
            }
        }

        // 获取所有单聊会话
        return privateChatSessionRepository->ofAll(sessionIds);
    }

    void PrivateChatSessionService::checkConfig()
    {
        if (privateChatSessionRepository == nullptr || participantRepository == nullptr)
        {
            throw sys::core::InfraException("PrivateChatSessionService is not properly configured.");
        }
    }
}
