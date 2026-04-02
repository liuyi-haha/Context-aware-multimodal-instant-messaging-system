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

        void update(const QString& userId, const QString& token, const QString& nickname = "",
                    const QString& avatarFileId = "",
                    const QString& description = "", const QString& phone = "")
        {
            QWriteLocker locker(&lock);
            currentUserId = userId;
            currentToken = token;
            this->nickname = nickname;
            this->avatarFileId = avatarFileId;
            this->description = description;
            this->phone = phone;
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

        QString getNickname() const
        {
            QReadLocker locker(&lock);
            return nickname;
        }

        QString getAvatarFileId() const
        {
            QReadLocker locker(&lock);
            return avatarFileId;
        }

    private:
        UserCredentialManager() = default;

    private:
        mutable QReadWriteLock lock;
        QString currentUserId;
        QString currentToken;
        QString nickname;
        QString avatarFileId;
        QString description;
        QString phone;
    };
}

