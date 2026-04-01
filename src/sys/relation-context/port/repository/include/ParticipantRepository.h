#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QList>

#include "sys/relation-context/domain/object/include/Participant.h"

namespace sys::relation::port
{
    class ParticipantRepository : public QObject
    {
        Q_OBJECT

    public:
        virtual ~ParticipantRepository() = default;
        virtual void saveAll(const QVector<QSharedPointer<domain::Participant>>& participants) = 0;
        virtual QSharedPointer<domain::Participant> of(const QString& participantId) = 0;
        virtual QSharedPointer<domain::Participant> of(const QString& privateChatSessionId, const QString& userId) = 0;
        virtual QList<QSharedPointer<domain::Participant>> ofChatSessionId(const QString& chatSessionId) = 0;
        virtual QList<QSharedPointer<domain::Participant>> ofAll(const QString& userId, domain::ParticipantRole role) =
        0;
    };
}
