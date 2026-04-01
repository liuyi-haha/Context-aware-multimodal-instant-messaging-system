//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QSharedPointer>
#include <QString>
#include "sys/common/Avatar.hpp"

namespace sys::user::domain
{
    class Phone
    {
    public:
        explicit Phone(const QString& phone)
            : phone(phone)
        {
        }

        static bool checkPhone(const QString& nickname);

        QString value() const
        {
            return phone;
        }

    private:
        const QString phone;
    };

    class Nickname
    {
    public:
        explicit Nickname(const QString& name)
            : name(name)
        {
        }

        static bool checkNickname(const QString& nickname);

        QString value() const
        {
            return name;
        }

    private:
        const QString name;
    };

    class Description
    {
    public:
        explicit Description(const QString& description)
            : description(description)
        {
        }

        QString value() const
        {
            return description;
        }

    private:
        const QString description;
    };

    class User
    {
    public:
        static QSharedPointer<User> of(const QString& id, const QString& nickname, const QString& phone,
                                       const QByteArray& avatar, const QString& avatarFileId,
                                       const QString& description = "");

        static bool checkAccount(const QString& account);
        static bool checkId(const QString& id);
        QString userId();
        QString avatarFileId();
        QString nicknameValue();
        QString phoneValue();
        QString descriptionValue();

    private:
        User(const QString& userId, const QString& nickname, const QString& phone, const QByteArray& avatar,
             const QString& avatarFileId, const QString& description)
            : id(userId),
              phone(phone),
              nickname(nickname),
              description(description),
              avatar(avatarFileId, avatar)
        {
        }

    private:
        QString id;
        Phone phone;
        Nickname nickname;
        Description description;
        core::Avatar avatar;
    };
}
