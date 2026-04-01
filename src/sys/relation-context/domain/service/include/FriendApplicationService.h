//
// Created by 86150 on 2026/3/25.
//
#pragma once
#include <QHash>
#include <QObject>
#include <QString>

#include "dependencyinjector.h"
#include "sys/relation-context/domain/event/FriendApplicationAccepted.h"
#include "sys/relation-context/domain/service/include/FriendApplicationValidator.h"
#include "sys/relation-context/port/client/include/BackendClient.h"
#include "sys/relation-context/port/repository/include/FriendApplicationRepository.h"
#include "sys/relation-context/port/repository/include/FriendShipRepository.h"

namespace sys::relation::domain
{
    class FriendApplicationService : public QObject
    {
        Q_OBJECT

    public:
        explicit FriendApplicationService(
            port::BackendClient* backendClient = QInjection::Inject,
            FriendApplicationValidator* friendApplicationValidator = QInjection::Inject,
            port::FriendApplicationRepository* friendApplicationRepository = QInjection::Inject,
            port::FriendShipRepository* friendShipRepository = QInjection::Inject,
            QObject* parent = nullptr);

        QString sendFriendApplication(const QString& targetUserId,
                                      const QString& verificationMessage,
                                      const QString& recipientRemark);

        void acceptFriendApplication(const QString& friendApplicationId, const QString& remark);
        void rejectFriendApplication(const QString& friendApplicationId);
        QList<QSharedPointer<FriendApplication>> getFriendApplications();

        void publish(const FriendApplicationAccepted& friendApplicationAccepted);
        // 应当在所有领域服务注册完毕后调用此方法
        void registerAll();
        void registerFriendApplicationAcceptedEventSubscriber(FriendApplicationAcceptedEventSubscriber* subscriber);

    private:
        port::BackendClient* backendClient = nullptr;
        FriendApplicationValidator* friendApplicationValidator = nullptr;
        port::FriendApplicationRepository* friendApplicationRepository = nullptr;
        port::FriendShipRepository* friendShipRepository = nullptr;
        QList<FriendApplicationAcceptedEventSubscriber*> friendApplicationAcceptedSubscribers;

    private:
        void checkConfig() const;
    };
}
