//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QString>

namespace sys::auth::domain
{
    class Password
    {
    public:
        static Password of(const QString& password);
        QString hashedValue() const;
        QString value() const;
        static bool checkPassword(const QString& password);

    private:
        explicit Password(const QString& password);
        const QString password;
    };

    class Token
    {
    public:
        static Token of(const QString& content);
        QString value() const;

    private:
        explicit Token(const QString& content);
        const QString content;
    };

    class UserCredential
    {
    public:
        static UserCredential of(const QString& userId, const Token& token);
        QString getUserId() const;
        const Token& getToken() const;

    private:
        UserCredential(QString userId, Token token);
        const QString userId;
        const Token token;
    };
} // namespace sys::auth::domain
