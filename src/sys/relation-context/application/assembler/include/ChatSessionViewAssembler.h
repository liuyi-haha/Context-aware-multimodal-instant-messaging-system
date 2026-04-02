//
// Created by 86150 on 2026/3/30.
//

#pragma once
#include "contract/system-provider/relation-context-provider/ChatSessionView.h"
#include "sys/relation-context/domain/object/include/ChatSession.h"
#include <QSharedPointer>
#include <QObject>
#include "dependencyinjector.h"
#include "sys/relation-context/domain/object/include/PrivateChatSession.h"
#include "sys/relation-context/port/client/include/MessageClient.h"
#include "sys/relation-context/port/client/include/NotificationClient.h"
#include "sys/common/port/UserClient.h"
#include "sys/relation-context/port/repository/include/FriendShipRepository.h"
#include "sys/relation-context/port/repository/include/ParticipantRepository.h"

namespace sys::relation::application
{
    class ChatSessionViewAssembler : public QObject
    {
        Q_OBJECT

    public:
        ChatSessionViewAssembler(port::MessageClient* messageClient = QInjection::Inject,
                                 port::NotificationClient* notificationClient = QInjection::Inject,
                                 port::ParticipantRepository* participantRepository = QInjection::Inject,
                                 common::port::UserClient* userClient = QInjection::Inject,
                                 QObject* parent = nullptr)
            : messageClient(messageClient),
              notificationClient(notificationClient),
              participantRepository(participantRepository),
              userClient(userClient),
              QObject(parent)
        {
        }

        // 暂时只支持单聊会话，后续再扩展支持群聊会话
        QList<contract::relation::ChatSessionView> toChatSessionViewsByLastMessageSendTimeDesc(
            const QList<QSharedPointer<domain::PrivateChatSession>>& privatechatSessions);

    private:
        QList<contract::relation::ChatSessionView> toChatSessionViews(
            const QList<QSharedPointer<domain::PrivateChatSession>>& chatSessions);

        void checkConfig() const;

    private:
        port::MessageClient* messageClient = nullptr;
        port::NotificationClient* notificationClient = nullptr;
        port::ParticipantRepository* participantRepository = nullptr;
        common::port::UserClient* userClient = nullptr;
    };
}
