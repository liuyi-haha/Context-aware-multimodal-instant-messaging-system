//
// Created by 86150 on 2026/4/10.
//
#include "RelationApplicationServiceTestFixture.h"
#include "notifications.pb.h"
#include <QByteArray>
TEST_F(RelationApplicationServiceTestFixture,
       处理好友结果通知事件时_如果结果为同意且新建了好友关系_本地仓储中好友申请状态应该变更为Accepted_本地仓储应该新增好友关系_本地仓储应该新增单聊会话_本地仓储应该新增两个聊天会话成员_同意好友申请应该成功)
{
    seedPendingApplication("FA-PENDING-OK", "100000002", "100000001", "验证消息", "给好友的备注");

    EXPECT_EQ(friendShipRepository->of("100000001", "100000002"), nullptr);


    proto::notification::FriendApplicationHandledNotification notification;
    notification.set_application_id("FA-PENDING-OK");
    notification.set_result_type(proto::notification::HandleFriendApplicationResultType::HANDLE_RESULT_APPROVED);
    notification.set_is_new_friendship(true);
    notification.set_friendship_id("100000001-100000002");
    notification.set_private_chat_session_id("PC-100000001-100000002");
    notification.set_applicant_participant_id("PC-100000001");
    notification.set_target_user_participant_id("PC-100000002");


    relationApplicationService->handleFriendApplicationHandled(
        QByteArray(notification.SerializeAsString().data(), notification.SerializeAsString().size())
    );


    const auto local = friendApplicationRepository->of("FA-PENDING-OK");
    ASSERT_NE(local, nullptr);
    EXPECT_EQ(local->applicationStatus(), sys::relation::domain::ApplicationStatus::Accepted);

    const auto friendShip = friendShipRepository->of("100000001", "100000002");
    ASSERT_NE(friendShip, nullptr);
    EXPECT_EQ(friendShip->friendShipId(), QString::fromStdString(notification.friendship_id()));

    const auto privateChatSession = privateChatSessionRepository->ofFriendShipId(friendShip->friendShipId());
    ASSERT_NE(privateChatSession, nullptr);

    const auto currentUserParticipant = participantRepository->of(
        privateChatSession->privateChatSessionId(),
        "100000001");
    ASSERT_NE(currentUserParticipant, nullptr);
    ASSERT_FALSE(currentUserParticipant->hasRemark()); // 无法查看对方给自己的备注

    auto targetParticipant = participantRepository->of(
        privateChatSession->privateChatSessionId(),
        "100000002");
    ASSERT_NE(targetParticipant, nullptr);
    ASSERT_TRUE(targetParticipant->hasRemark());
    ASSERT_EQ(targetParticipant->remarkValue(), QString("给好友的备注"));
    qDebug() << targetParticipant->remarkValue() << "===" << QString("给好友的备注");
}
