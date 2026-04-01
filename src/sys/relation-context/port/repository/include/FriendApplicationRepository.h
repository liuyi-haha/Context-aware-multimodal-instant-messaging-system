//
// Created by 86150 on 2026/3/25.
//
#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QString>

#include "sys/relation-context/domain/object/include/FriendApplication.h"

namespace sys::relation::port
{
    class FriendApplicationRepository : public QObject
    {
        Q_OBJECT

    public:
        virtual ~FriendApplicationRepository() = default;
        virtual QSharedPointer<domain::FriendApplication> of(
            const QString& applicantUserId,
            const QString& targetUserId,
            domain::ApplicationStatus status) = 0;
        virtual QSharedPointer<domain::FriendApplication> of(const QString& friendApplicationId) = 0;
        virtual QList<QSharedPointer<domain::FriendApplication>> ofApplicantUserId(const QString& applicantUserId) = 0;
        virtual QList<QSharedPointer<domain::FriendApplication>> ofTargetUserId(const QString& ofTargetUserId) = 0;
        virtual void save(QSharedPointer<domain::FriendApplication> friendApplication) = 0;
    };
}
