#include "relation-context/application/service/RelationApplicationServiceTestFixture.h"

//------------------------------- 同意好友申请 -------------------------------
// rule:
// 已经处理过的好友申请不能再被处理
// 只有好友申请的目标用户是自己时，才允许处理好友申请
// 同意好友申请后，好友申请的状态应该变更为Accepted
// 如果之前没有好友关系，那么同意后应该建立好友关系，否则不需要重复建立好友关系
// 建立好友关系后，应该建立和好友关系绑定的单聊会话
// 建立单聊会话后，两人都成为单聊会话的成员
TEST_F(RelationApplicationServiceTestFixture, 同意好友申请时_如果好友申请状态为Accepted_应该给出提示)
{
    seedAcceptedApplication("FA-ACC-001", "100000002", "100000001");

    const auto response = relationApplicationService->acceptFriendApplication(
        buildAcceptRequest("FA-ACC-001", "给好友的备注"));

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "好友申请已被处理");
    EXPECT_EQ(chatApiGatewayFake.acceptCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture, 同意好友申请时_如果好友申请状态为Rejected_应该给出提示)
{
    seedRejectedApplication("FA-REJ-001", "100000002", "100000001");

    const auto response = relationApplicationService->acceptFriendApplication(
        buildAcceptRequest("FA-REJ-001", "给好友的备注"));

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "好友申请已被处理");
    EXPECT_EQ(chatApiGatewayFake.acceptCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture, 同意好友申请时_如果处理好友申请的用户不是目标用户_应该给出提示)
{
    seedPendingApplication("FA-PENDING-UNAUTH", "100000002", "100000003");

    const auto response = relationApplicationService->acceptFriendApplication(
        buildAcceptRequest("FA-PENDING-UNAUTH", "给好友的备注"));

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "无权处理该好友申请");
    EXPECT_EQ(chatApiGatewayFake.acceptCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture,
       同意好友申请时_如果申请用户和当前用户之间不存在好友关系_本地仓储中好友申请状态应该变更为Accepted_本地仓储应该新增好友关系_本地仓储应该新增单聊会话_本地仓储应该新增两个聊天会话成员_同意好友申请应该成功)
{
    seedPendingApplication("FA-PENDING-OK", "100000002", "100000001", "验证消息", "给好友的备注");

    EXPECT_EQ(friendShipRepository->of("100000001", "100000002"), nullptr);

    const auto response = relationApplicationService->acceptFriendApplication(
        buildAcceptRequest("FA-PENDING-OK", "给好友的备注"));

    ASSERT_TRUE(chatApiGatewayFake.lastAcceptOutcome().has_value());
    const auto acceptOutcome = chatApiGatewayFake.lastAcceptOutcome().value();
    ASSERT_TRUE(acceptOutcome.createFriendShip);

    EXPECT_TRUE(response.success);
    EXPECT_EQ(chatApiGatewayFake.acceptCallCount, 1);

    const auto local = friendApplicationRepository->of("FA-PENDING-OK");
    ASSERT_NE(local, nullptr);
    EXPECT_EQ(local->applicationStatus(), sys::relation::domain::ApplicationStatus::Accepted);

    const auto friendShip = friendShipRepository->of("100000001", "100000002");
    ASSERT_NE(friendShip, nullptr);
    EXPECT_EQ(friendShip->friendShipId(), acceptOutcome.friendShipId);

    const auto privateChatSession = privateChatSessionRepository->ofFriendShipId(friendShip->friendShipId());
    ASSERT_NE(privateChatSession, nullptr);

    const auto currentUserParticipant = participantRepository->of(
        privateChatSession->privateChatSessionId(),
        "100000001");
    ASSERT_NE(currentUserParticipant, nullptr);
    ASSERT_FALSE(currentUserParticipant->hasRemark()); // 无法查看对方给自己的备注

    auto applicantParticipant = participantRepository->of(
        privateChatSession->privateChatSessionId(),
        "100000002");
    ASSERT_NE(applicantParticipant, nullptr);
    ASSERT_TRUE(applicantParticipant->hasRemark());
    ASSERT_EQ(applicantParticipant->remarkValue(), "给好友的备注");
}

TEST_F(RelationApplicationServiceTestFixture,
       同意好友申请时_如果申请用户和当前用户之间已经存在好友关系_本地仓储中好友申请状态应该变更为Accepted_本地仓储中不应该新增好友关系_本地仓储不应该新增单聊会话_本地仓储不应该新增聊天会话成员_同意好友申请应该成功)
{
    chatApiGatewayFake.seedFriendShip("100000001", "100000002");
    friendShipRepository->save(
        sys::relation::domain::FriendShip::of("FS-EXIST-001", "100000001", "100000002"));
    seedPendingApplication("FA-PENDING-EXIST", "100000002", "100000001");

    const auto response = relationApplicationService->acceptFriendApplication(
        buildAcceptRequest("FA-PENDING-EXIST", "给好友的备注"));

    ASSERT_TRUE(chatApiGatewayFake.lastAcceptOutcome().has_value());
    const auto acceptOutcome = chatApiGatewayFake.lastAcceptOutcome().value();
    ASSERT_FALSE(acceptOutcome.createFriendShip);

    EXPECT_TRUE(response.success);
    EXPECT_EQ(chatApiGatewayFake.acceptCallCount, 1);

    const auto local = friendApplicationRepository->of("FA-PENDING-EXIST");
    ASSERT_NE(local, nullptr);
    EXPECT_EQ(local->applicationStatus(), sys::relation::domain::ApplicationStatus::Accepted);

    const auto friendShip = friendShipRepository->of("100000001", "100000002");
    ASSERT_NE(friendShip, nullptr);
    EXPECT_EQ(friendShip->friendShipId(), "FS-EXIST-001");

    EXPECT_EQ(privateChatSessionRepository->ofFriendShipId(friendShip->friendShipId()), nullptr);
    EXPECT_EQ(participantRepository->of("PCS-EXIST-001", "100000001"), nullptr);
    EXPECT_EQ(participantRepository->of("PCS-EXIST-001", "100000002"), nullptr);
}

