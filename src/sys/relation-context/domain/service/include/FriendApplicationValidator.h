#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include "dependencyinjector.h"
#include "dependencypool.h"
#include "sys/relation-context/domain/object/include/FriendApplication.h"
#include "sys/relation-context/port/repository/include/FriendApplicationRepository.h"
#include "sys/relation-context/port/repository/include/FriendShipRepository.h"

namespace sys::relation::domain
{
    class FriendApplicationValidator : public QObject
    {
        Q_OBJECT

    public:
        explicit FriendApplicationValidator(
            port::FriendApplicationRepository* friendApplicationRepository = QInjection::Inject,
            port::FriendShipRepository* friendShipRepository = QInjection::Inject,
            QObject* parent = nullptr);

        void validateSendFriendApplicationInputs(
            const QString& verificationMessage,
            const QString& recipientRemark,
            const QString& targetUserId) const;

        QSharedPointer<FriendApplication> validateHandleFriendApplicationInputs(
            const QString& friendApplicationId,
            const QString& currentUserId) const;

    private:
        port::FriendApplicationRepository* friendApplicationRepository = nullptr;
        port::FriendShipRepository* friendShipRepository = nullptr;

    private:
        // 校验好友申请是否已经被处理
        void validateIsProcessed(const QSharedPointer<FriendApplication>& friendApplication) const;
        // 校验好友申请是否存在，并返回好友申请
        QSharedPointer<FriendApplication> validateExistence(const QString& friendApplicationId) const;
        // 校验是否已经存在未处理的好友申请
        void validatePendingApplicationExistence(const QString& currentUserId, const QString& targetUserId) const;
        // 校验是否已经是好友关系
        void validateFriendShipExistence(const QString& currentUserId, const QString& targetUserId) const;
    };
}
