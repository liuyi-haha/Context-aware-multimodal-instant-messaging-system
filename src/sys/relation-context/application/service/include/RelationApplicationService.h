//
// Created by 86150 on 2026/3/25.
//
#pragma once
#include <QObject>

#include "contract/system-provider/relation-context-provider/AcceptFriendApplication.h"
#include "dependencyinjector.h"
#include "contract/system-provider/relation-context-provider/ChatSessionView.h"
#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"
#include "contract/system-provider/relation-context-provider/RejectFriendApplication.h"
#include "contract/system-provider/relation-context-provider/SendFriendApplication.h"
#include "sys/relation-context/application/assembler/include/ChatSessionViewAssembler.h"
#include "sys/relation-context/domain/service/include/FriendApplicationService.h"
#include "sys/relation-context/application/assembler/include/FriendApplicationViewAssembler.h"
#include "sys/relation-context/domain/service/include/PrivateChatSessionService.h"

namespace sys::relation::application
{
    class RelationApplicationService : public QObject
    {
        Q_OBJECT

    public:
        explicit RelationApplicationService(
            domain::FriendApplicationService* friendApplicationService = QInjection::Inject,
            FriendApplicationViewAssembler* friendApplicationViewAssembler = QInjection::Inject,
            domain::PrivateChatSessionService* privateChatSessionService = QInjection::Inject,
            ChatSessionViewAssembler* chatSessionViewAssembler = QInjection::Inject,
            QObject* parent = nullptr);

        contract::relation::SendFriendApplicationResponse sendFriendApplication(
            const contract::relation::SendFriendApplicationRequest& request);

        // 同意好友申请
        contract::relation::AcceptFriendApplicationResponse acceptFriendApplication(
            const contract::relation::AcceptFriendApplicationRequest& request);

        // 拒绝好友申请
        contract::relation::RejectFriendApplicationResponse rejectFriendApplication(
            const contract::relation::RejectFriendApplicationRequest& request);

        // 获取所有好友申请
        QList<contract::relation::FriendApplicationView> getFriendApplications();

        // 获取所有聊天会话: 暂时只获取单聊会话，群聊会话后续再扩展
        QList<contract::relation::ChatSessionView> getChatSessions();

        void checkSenderHasPermissionToSendMessage(const QString& chatSessionId); //有问题直接抛异常，没有问题则正常返回

        enum class NoPermissionReason
        {
            ChatSessionNotFound,
            NotParticipant,
            NotFriend,
            Other
        };

        struct CheckSendMessagePermissionResponse
        {
            bool hasPermissionToSendMessage;
            std::optional<NoPermissionReason> noPermissionReason;
        };

        CheckSendMessagePermissionResponse checkSendMessagePermission();

    private:
        domain::FriendApplicationService* friendApplicationService = nullptr;
        FriendApplicationViewAssembler* friendApplicationViewAssembler = nullptr;
        domain::PrivateChatSessionService* privateChatSessionService = nullptr;
        ChatSessionViewAssembler* chatSessionViewAssembler = nullptr;

    private:
        bool checkConfigAndSetResponse(contract::BaseResponse& response);
        void checkConfig() const;
    };
}
