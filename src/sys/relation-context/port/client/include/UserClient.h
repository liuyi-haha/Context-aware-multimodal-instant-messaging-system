//
// Created by 86150 on 2026/3/27.
//

#pragma once
#include <QString>
#include <QHash>
#include <QList>
#include <QObject>

namespace sys::relation::port
{
    struct UserInfo
    {
        QString userId;
        QString nickname;
        QString avatarFileId;
    };

    class UserClient : public QObject
    {
        Q_OBJECT

    public:
        virtual ~UserClient() = default;
        virtual UserInfo getUser(const QString& userId) = 0;
        virtual QHash<QString, UserInfo> getUsers(const QSet<QString>& userIds) = 0;
    };
}
