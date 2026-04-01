#pragma once

#include <QReadWriteLock>
#include <QString>

namespace sys::common::component
{
    // Process-wide login credential cache.
    class UserCredentialManager
    {
    public:
        static UserCredentialManager& instance()
        {
            static UserCredentialManager manager;
            return manager;
        }

        void update(const QString& userId, const QString& token)
        {
            QWriteLocker locker(&lock);
            currentUserId = userId;
            currentToken = token;
        }

        QString getCurrentUserId() const
        {
            QReadLocker locker(&lock);
            return currentUserId;
        }

        QString getCurrentToken() const
        {
            QReadLocker locker(&lock);
            return currentToken;
        }

    private:
        UserCredentialManager() = default;

    private:
        mutable QReadWriteLock lock;
        QString currentUserId;
        QString currentToken;
    };
}

