#include "relation-context/application/service/RelationApplicationServiceTestFixture.h"

//------------------------------- 拒绝好友申请 -------------------------------
// rule:
// 已经处理过的好友申请不能再被处理
// 只有好友申请的目标用户是自己时，才允许处理好友申请
// 拒绝好友申请后，好友申请的状态应该变更为Rejected
TEST_F(RelationApplicationServiceTest, 拒绝好友申请时_如果好友申请状态为Accepted_应该给出提示)
{
    seedAcceptedApplication("FA-REJECT-ACC", "100000002", "100000001");

    const auto response = relationApplicationService->rejectFriendApplication(buildRejectRequest("FA-REJECT-ACC"));

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "好友申请已被处理");
    EXPECT_EQ(chatApiGatewayFake.rejectCallCount, 0);
}

TEST_F(RelationApplicationServiceTest, 拒绝好友申请时_如果好友申请状态为Rejected_应该给出提示)
{
    seedRejectedApplication("FA-REJECT-REJ", "100000002", "100000001");

    const auto response = relationApplicationService->rejectFriendApplication(buildRejectRequest("FA-REJECT-REJ"));

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "好友申请已被处理");
    EXPECT_EQ(chatApiGatewayFake.rejectCallCount, 0);
}

TEST_F(RelationApplicationServiceTest, 拒绝好友申请时_如果处理好友申请的用户不是目标用户_应该给出提示)
{
    seedPendingApplication("FA-REJECT-UNAUTH", "100000002", "100000003");

    const auto response = relationApplicationService->rejectFriendApplication(buildRejectRequest("FA-REJECT-UNAUTH"));

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "无权处理该好友申请");
    EXPECT_EQ(chatApiGatewayFake.rejectCallCount, 0);
}

TEST_F(RelationApplicationServiceTest,
       拒绝好友申请时_如果一切合法_本地仓储中好友申请状态应该变更为Rejected_本地仓储不应该新增好友关系_拒绝好友申请应该成功)
{
    seedPendingApplication("FA-REJECT-OK", "100000002", "100000001");

    EXPECT_EQ(friendShipRepository->of("100000001", "100000002"), nullptr);

    const auto response = relationApplicationService->rejectFriendApplication(buildRejectRequest("FA-REJECT-OK"));

    EXPECT_TRUE(response.success);
    EXPECT_EQ(chatApiGatewayFake.rejectCallCount, 1);

    const auto local = friendApplicationRepository->of("FA-REJECT-OK");
    ASSERT_NE(local, nullptr);
    EXPECT_EQ(local->applicationStatus(), sys::relation::domain::ApplicationStatus::Rejected);

    EXPECT_EQ(friendShipRepository->of("100000001", "100000002"), nullptr);
}

