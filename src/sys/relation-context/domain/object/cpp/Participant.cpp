//
// Created by 86150 on 2026/3/22.
//

#include "sys/relation-context/domain/object/include/Participant.h"

#include "sys/common/exception/DomainException.h"

namespace sys::relation::domain
{
    Participant::Participant(const QString& participantId,
                             const QString& chatSessionId,
                             const QString& userId,
                             const ParticipantRole role,
                             const QString& remark)
        : participantId(participantId),
          chatSessionId(chatSessionId),
          userId(userId),
          role(role),
          remark(remark)
    {
    }

    Participant::Participant(const QString& participantId,
                             const QString& chatSessionId,
                             const QString& userId,
                             const ParticipantRole role)
        : participantId(participantId),
          chatSessionId(chatSessionId),
          userId(userId),
          role(role)

    {
    }


    QSharedPointer<Participant> Participant::of(const QString& participantId,
                                                const QString& chatSessionId,
                                                const QString& userId,
                                                const ParticipantRole role, const QString& remark)
    {
        return QSharedPointer<Participant>(new Participant(participantId, chatSessionId, userId, role, remark));
    }

    QSharedPointer<Participant> Participant::of(const QString& participantId, const QString& chatSessionId,
                                                const QString& userId, ParticipantRole role)
    {
        return QSharedPointer<Participant>(new Participant(participantId, chatSessionId, userId, role));
    }

    const QString& Participant::id() const
    {
        return participantId;
    }

    const QString& Participant::sessionId() const
    {
        return chatSessionId;
    }

    const QString& Participant::userIdValue() const
    {
        return userId;
    }

    ParticipantRole Participant::roleValue() const
    {
        return role;
    }

    QString Participant::remarkValue() const
    {
        if (!remark.has_value())
        {
            throw core::DomainException("无权查看备注");
        }

        return remark.value().value();
    }

    bool Participant::hasRemark() const
    {
        return remark.has_value();
    }
}
