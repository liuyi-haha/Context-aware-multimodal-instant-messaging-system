//
// Created by 86150 on 2026/3/22.
//

#include "../include/UserCredential.h"
#include "sys/auth-context/domain/exception/InvalidPasswordException.h"
#include <QCryptographicHash>
#include <QRegularExpression>

#include <utility>

namespace sys::auth::domain
{
    Password Password::of(const QString& password)
    {
        if (!checkPassword(password))
        {
            throw InvalidPasswordException();
        }
        return Password(password);
    }

    QString Password::hashedValue() const
    {
        const QByteArray digest = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        return QString::fromLatin1(digest.toHex());
    }

    QString Password::value() const
    {
        return password;
    }

    bool Password::checkPassword(const QString& password)
    {
        static const QRegularExpression passwordPattern(QStringLiteral("^[A-Za-z0-9_]{6,}$"));
        return passwordPattern.match(password).hasMatch();
    }

    Password::Password(const QString& password)
        : password(password)
    {
    }

    Token Token::of(const QString& content)
    {
        return Token(content);
    }

    QString Token::value() const
    {
        return content;
    }

    Token::Token(const QString& content)
        : content(content)
    {
    }

    UserCredential UserCredential::of(const QString& userId, const Token& token)
    {
        return UserCredential(userId, token);
    }

    QString UserCredential::getUserId() const
    {
        return userId;
    }

    const Token& UserCredential::getToken() const
    {
        return token;
    }

    UserCredential::UserCredential(QString userId, Token token)
        : userId(std::move(userId)), token(std::move(token))
    {
    }
}
