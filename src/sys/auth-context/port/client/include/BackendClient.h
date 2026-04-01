//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>
#include <QObject>

namespace sys::auth::port
{
    class BackendClient : public QObject
    {
        Q_OBJECT
    public:
        struct LoginResult
        {
            QString userId;
            QString token;
        };

        virtual ~BackendClient() = default;
        virtual LoginResult login(const QString& account, const QString& hashedPassword) = 0;
    };
} // namespace sys::auth::port