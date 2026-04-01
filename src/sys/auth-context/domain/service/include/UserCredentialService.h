//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QObject>
#include <QString>

#include "dependencyinjector.h"
#include "sys/auth-context/port/Client/include/BackendClient.h"

namespace sys::auth::domain
{
    class UserCredentialService : public QObject
    {
        Q_OBJECT

    public:
        UserCredentialService(
            port::BackendClient* backendClient = QInjection::Inject,
            QObject* parent = nullptr);
        QString login(const QString& account, const QString& password);

    private:
        port::BackendClient* backendClient = nullptr;
        void checkConfig() const;
    };
} // namespace sys::auth::domain
