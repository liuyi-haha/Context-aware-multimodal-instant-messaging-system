//
// Created by 86150 on 2026/3/23.
//

#pragma once
#include <QString>
#include <QSharedPointer>

#include "sys/user-context/domain/object/include/User.h"
#include <QObject>

namespace sys::user::port
{
    class BackendClient : public QObject
    {
        Q_OBJECT

    public:
        struct RegisterUserResult
        {
            QString userId;
            QString avatarFileId;
        };

        virtual ~BackendClient() = default;
        virtual RegisterUserResult registerUser(const QString& hashedPassword, const QString& nickname,
                                                const QString& phone, const QByteArray& avatar) = 0;
        virtual QSharedPointer<domain::User> searchUser(const QString& account) = 0;
    };
}
