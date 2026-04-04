//
// Created by 86150 on 2026/3/22.
//

#include "../include/User.h"

#include <QRegularExpression>

#include "sys/user-context/domain/exception/InvalidNicknameException.h"
#include "sys/user-context/domain/exception/InvalidPhoneException.h"
#include "sys/user-context/domain/exception/InvalidUserIdException.h"

namespace sys::user::domain
{
    bool Phone::checkPhone(const QString& nickname)
    {
        static const QRegularExpression phonePattern("^\\d{11}$");
        return phonePattern.match(nickname).hasMatch();
    }

    bool Nickname::checkNickname(const QString& nickname)
    {
        // 规则：长度 1~10，字符仅允许中文、英文和下划线
        static const QRegularExpression nicknamePattern("^[A-Za-z_\\x{4E00}-\\x{9FFF}]{1,10}$");
        return nicknamePattern.match(nickname).hasMatch();
    }

    QSharedPointer<User> User::of(const QString& id, const QString& nickname, const QString& phone,
                                  const QByteArray& avatar, const QString& avatarFileId,
                                  const QString& description)
    {
        if (!Nickname::checkNickname(nickname))
        {
            throw InvalidNicknameException();
        }
        if (!Phone::checkPhone(phone))
        {
            throw InvalidPhoneException();
        }
        if (!User::checkId(id))
        {
            throw InvalidUserIdException();
        }
        return QSharedPointer<User>(new User(id, nickname, phone, avatar, avatarFileId, description));
    }

    bool User::checkAccount(const QString& account)
    {
        // 账号允许手机号，或可识别的用户ID格式。
        return Phone::checkPhone(account) || User::checkId(account);
    }

    bool User::checkId(const QString& id)
    {
        // 最新规则：用户ID为9位数字字符串，且不能以0开头。
        static const QRegularExpression userIdPattern("^[1-9]\\d{8}$");
        return userIdPattern.match(id).hasMatch();
    }

    QString User::userId()
    {
        return id;
    }

    QString User::avatarFileId()
    {
        return avatar.avatarFileId();
    }

    QString User::nicknameValue()
    {
        return nickname.value();
    }

    QString User::phoneValue()
    {
        return phone.value();
    }

    QString User::descriptionValue()
    {
        return description.value();
    }
}
