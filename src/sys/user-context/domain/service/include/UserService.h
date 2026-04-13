//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>

#include "dependencyinjector.h"
#include "sys/user-context/port/client/include/BackendClient.h"
#include "sys/user-context/port/client/include/FileClient.h"
#include "sys/user-context/domain/service/include/UserValidator.h"
#include "sys/user-context/port/repository/include/UserRepository.h"


namespace sys
{
    namespace user::domain
    {
        class UserService : public QObject
        {
            Q_OBJECT

        public:
            UserService(
                UserValidator* userValidator = QInjection::Inject,
                port::BackendClient* backendClient = QInjection::Inject,
                port::FileClient* fileClient = QInjection::Inject,
                port::UserRepository* userRepository = QInjection::Inject);
            QString registerUser(const QString& nickname, const QString& phone,
                                 const QString& password,
                                 const QFileInfo& avatarFileInfo);
            QSharedPointer<domain::User> searchUser(const QString& account);
            QSharedPointer<domain::User> getUser(const QString& userId);
            QList<QSharedPointer<domain::User>> getUsers(const QSet<QString>& userIds);

        private:
            UserValidator* userValidator = nullptr;
            port::BackendClient* backendClient = nullptr;
            port::FileClient* fileClient = nullptr;
            port::UserRepository* userRepository = nullptr;
            void checkConfig(bool requireUserValidator, bool requireFileClient) const;
        };
    }
} // sys
