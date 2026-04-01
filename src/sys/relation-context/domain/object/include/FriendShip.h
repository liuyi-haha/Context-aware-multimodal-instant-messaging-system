//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QSharedPointer>
#include <QString>

#include "sys/common/RecipientRemark.hpp"

namespace sys::relation::domain
{
    class FriendShip
    {
    public:
        static QSharedPointer<FriendShip> of(const QString& friendShipId,
                                             const QString& userId,
                                             const QString& friendUserId);

        const QString& friendShipId() const;
        const QString& ownerUserId() const;
        const QString& peerUserId() const;
        const QString& recipientRemarkValue() const;

    private:
        FriendShip(const QString& friendShipId,
                   const QString& userId,
                   const QString& friendUserId);

    private:
        QString friendshipId;
        QString userId;
        QString peerId;
    };
}
