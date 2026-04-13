//
// Created by 86150 on 2026/3/26.
//

#include "../include/ParticipantService.h"

#include <QVector>

#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"
#include "sys/relation-context/domain/object/include/Participant.h"

namespace sys::relation::domain
{
    ParticipantService::ParticipantService(
        port::ParticipantRepository* participantRepository,
        QObject* parent)
        : QObject(parent),
          participantRepository(participantRepository)
    {
    }

    void ParticipantService::handle(FriendApplicationAccepted event)
    {
        checkConfig();

        // 先搞清楚谁是申请人，谁是目标用户
        QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();
        if (currentUserId == event.applicantUserId)
        {
            // 我是申请人，对方是目标用户，所以备注是给目标用户的
            auto applicantParticipant = Participant::of(event.applicantParticipantId,
                                                        event.privateChatSessionId,
                                                        event.applicantUserId,
                                                        ParticipantRole::PrivateMember); // 对方的备注我无权查看
            auto targetParticipant = Participant::of(event.targetUserParticipantId,
                                                     event.privateChatSessionId,
                                                     event.targetUserId,
                                                     ParticipantRole::PrivateMember,
                                                     event.remark);
            participantRepository->saveAll({applicantParticipant, targetParticipant});
        }
        else
        {
            // 我是目标用户，对方是申请人，所以备注是给申请人的
            auto applicantParticipant = Participant::of(event.applicantParticipantId,
                                                        event.privateChatSessionId,
                                                        event.applicantUserId,
                                                        ParticipantRole::PrivateMember); // 我给对方的备注我无权查看
            auto targetParticipant = Participant::of(event.targetUserParticipantId,
                                                     event.privateChatSessionId,
                                                     event.targetUserId,
                                                     ParticipantRole::PrivateMember, event.remark); // 对方给我的备注

            participantRepository->saveAll({applicantParticipant, targetParticipant});
        }


        if (event.applicantUserId == currentUserId)
        {
            return;
        }
        // 当前是我同意好友申请，我是目标用户
        auto applicantParticipant = Participant::of(event.applicantParticipantId,
                                                    event.privateChatSessionId,
                                                    event.applicantUserId,
                                                    ParticipantRole::PrivateMember, event.remark); // 我给对方的备注
        auto targetParticipant = Participant::of(event.targetUserParticipantId,
                                                 event.privateChatSessionId,
                                                 event.targetUserId,
                                                 ParticipantRole::PrivateMember); // 对方给我的备注我无权查看

        participantRepository->saveAll({applicantParticipant, targetParticipant});
    }

    void ParticipantService::checkConfig()
    {
        if (participantRepository == nullptr)
        {
            throw core::InfraException("ParticipantService is not configured.");
        }
    }
}
