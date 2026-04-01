//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QObject>
#include <QString>

#include "sys/auth-context/domain/service/include/UserCredentialService.h"
#include "contract/system-provider/auth-context-provider/Login.h"

namespace sys::auth::application
{
    class AuthApplicationService : public QObject
    {
        Q_OBJECT

    public:
        explicit AuthApplicationService(domain::UserCredentialService* userCredentialService = QInjection::Inject,
                                        QObject* parent = nullptr);
        bool validatePassword(const QString& password);
        QString getHashedPassword(const QString& password);
        contract::auth::LoginResponse login(const contract::auth::LoginRequest& request);

    private:
        domain::UserCredentialService* userCredentialService = nullptr;

    private:
        bool checkConfigAndSetResponse(contract::BaseResponse& response);
    };
} // namespace sys::auth::application
