//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include "sys/relation-context/domain/object/include/FriendShip.h"

namespace sys::relation::port
{
    class FriendShipRepository : public QObject
    {
        Q_OBJECT

    public:
        virtual ~FriendShipRepository() = default;
        virtual QSharedPointer<domain::FriendShip> of(const QString& currentUserId, const QString& peerUserId) = 0;
        virtual QSharedPointer<domain::FriendShip> of(const QString& friendShipId) = 0;
        virtual void save(const QSharedPointer<domain::FriendShip>& friendShip) = 0;
    };
}
