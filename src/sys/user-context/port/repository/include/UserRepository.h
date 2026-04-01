//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QSharedPointer>

#include "sys/user-context/domain/object/include/User.h"
#include <QObject>

namespace sys
{
    namespace user::port
    {
        class UserRepository : public QObject
        {
            Q_OBJECT

        public:
            virtual void save(QSharedPointer<domain::User> user) = 0;
            virtual QSharedPointer<domain::User> of(const QString& userId) = 0;
            virtual QList<QSharedPointer<domain::User>> ofAll(const QSet<QString>& userIds) = 0;
        };
    }
} // sys
