#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QSet>

#include "sys/relation-context/domain/object/include/PrivateChatSession.h"

namespace sys::relation::port
{
    class PrivateChatSessionRepository : public QObject
    {
        Q_OBJECT

    public:
        virtual ~PrivateChatSessionRepository() = default;
        virtual void save(const QSharedPointer<domain::PrivateChatSession>& privateChatSession) = 0;
        virtual QSharedPointer<domain::PrivateChatSession> of(const QString& privateChatSessionId) = 0;
        virtual QSharedPointer<domain::PrivateChatSession> ofFriendShipId(const QString& friendShipId) = 0;
        virtual QList<QSharedPointer<domain::PrivateChatSession>> ofAll(const QSet<QString>& privateChatSessionIds) = 0;
    };
}
