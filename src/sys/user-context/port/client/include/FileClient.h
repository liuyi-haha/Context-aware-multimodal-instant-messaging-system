//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QSharedPointer>

#include "sys/user-context/domain/object/include/User.h"
#include <QObject>

namespace sys::user::port
{
    class FileClient : public QObject
    {
        Q_OBJECT
    public:
        virtual ~FileClient() = default;
        virtual void uploadAvatar(const QSharedPointer<domain::User>& user) = 0;
    };
}
