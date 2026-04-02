//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include <QObject>

#include "dependencypointer.h"
#include "dependencyinjector.h"
#include "sys/relation-context/domain/event/FriendApplicationAccepted.h"
#include "sys/relation-context/port/repository/include/FriendShipRepository.h"
#include "sys/relation-context/port/repository/include/PrivateChatSessionRepository.h"
#include "sys/relation-context/port/repository/include/ParticipantRepository.h"

namespace sys::relation::domain
{
    class PrivateChatSessionService : public QObject, public FriendApplicationAcceptedEventSubscriber
    {
        Q_OBJECT

    public:
        explicit PrivateChatSessionService(
            port::PrivateChatSessionRepository* privateChatSessionRepository = QInjection::Inject,
            port::ParticipantRepository* participantRepository = QInjection::Inject,
            port::FriendShipRepository* friendshipRepository = QInjection::Inject,
            QObject* parent = nullptr);

        void handle(FriendApplicationAccepted friendApplicationAccepted) override;

        QList<QSharedPointer<PrivateChatSession>> getPrivateChatSessions();
        void checkCurrentUserHasPermissionToSendMessage(const QString& chatSessionId); // 如果没有权限则抛异常

    private:
        port::PrivateChatSessionRepository* privateChatSessionRepository = nullptr;
        port::ParticipantRepository* participantRepository = nullptr;
        port::FriendShipRepository* friendShipRepository = nullptr;

    private:
        void checkConfig();
    };
}
