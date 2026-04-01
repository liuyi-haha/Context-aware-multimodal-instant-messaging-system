//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QSharedPointer>
#include <QString>

#include "sys/relation-context/domain/object/include/ChatSession.h"

namespace sys::relation::domain
{
    class PrivateChatSession : public ChatSession
    {
    public:
        static QSharedPointer<PrivateChatSession> of(const QString& privateChatSessionId,
                                                     const QString& friendShipId);

        const QString& privateChatSessionId() const;
        const QString& friendShipId() const;
        ChatSessionType getType() const override;

    private:
        PrivateChatSession(const QString& privateChatSessionId,
                           const QString& friendShipId);

    private:
        QString id;
        QString friendshipId;
    };
}
