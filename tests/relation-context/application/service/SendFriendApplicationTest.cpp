#include "relation-context/application/service/RelationApplicationServiceTestFixture.h"

//------------------------------- 发送好友申请 -------------------------------
// rule:
// 好友申请的验证消息长度限制为[0, 20]，字符类型不限
// 好友申请的对方备注长度限制为[0, 20]，字符类型不限
// 申请用户不能在好友申请未被处理前，再次向同一用户发送好友申请
// 申请用户和目标用户不能已经是好友关系，否则不允许发送好友申请
// 申请用户和目标用户不能是同一个人
// 好友申请发送成功后，应当在本地仓储新增一条待处理的好友申请记录
TEST_F(RelationApplicationServiceTestFixture, 发送好友申请时_如果验证消息长度不合法_应该给出提示)
{
    auto request = buildValidSendFriendApplicationRequest();
    request.verificationMessage = "aaaaaaaaaaaaaaaaaaaaa";

    const auto response = relationApplicationService->sendFriendApplication(request);

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "验证消息长度不合法");
    EXPECT_EQ(chatApiGatewayFake.sendCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture, 发送好友申请时_如果对方备注长度不合法_应该给出提示)
{
    auto request = buildValidSendFriendApplicationRequest();
    request.recipientRemark = "bbbbbbbbbbbbbbbbbbbbb";

    const auto response = relationApplicationService->sendFriendApplication(request);

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "对方备注长度不合法");
    EXPECT_EQ(chatApiGatewayFake.sendCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture, 发送好友申请时_如果存在发送给目标用户且状态为Pending状态的好友申请时_应该给出提示)
{
    seedPendingApplication("100000002");

    const auto response = relationApplicationService->sendFriendApplication(buildValidSendFriendApplicationRequest());

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "已存在待处理的好友申请");
    EXPECT_EQ(chatApiGatewayFake.sendCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture, 发送好友申请时_如果当前用户和目标用户已经是好友_应该给出提示)
{
    friendShipRepository->save(sys::relation::domain::FriendShip::of("FS-SEED-001", "100000001", "100000002"));

    const auto response = relationApplicationService->sendFriendApplication(buildValidSendFriendApplicationRequest());

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "双方已经是好友");
    EXPECT_EQ(chatApiGatewayFake.sendCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture, 发送好友申请时_如果当前用户和目标用户是同一个人_应该给出提示)
{
    auto request = buildValidSendFriendApplicationRequest();
    request.targetUserId = "100000001";

    const auto response = relationApplicationService->sendFriendApplication(request);

    EXPECT_FALSE(response.success);
    ASSERT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "不能添加自己为好友");
    EXPECT_EQ(chatApiGatewayFake.sendCallCount, 0);
}

TEST_F(RelationApplicationServiceTestFixture, 发送好友申请时_如果一切合法_应该成功得到好友申请ID并在本地仓储新增一条待处理的好友申请记录)
{
    const auto request = buildValidSendFriendApplicationRequest();
    const auto response = relationApplicationService->sendFriendApplication(request);

    ASSERT_TRUE(chatApiGatewayFake.lastGeneratedFriendApplicationId().has_value());
    const QString generatedFriendApplicationId = chatApiGatewayFake.lastGeneratedFriendApplicationId().value();

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.friendApplicationId, generatedFriendApplicationId);
    EXPECT_EQ(chatApiGatewayFake.sendCallCount, 1);

    EXPECT_EQ(chatApiGatewayFake.lastRequest.getTargetUserId(), "100000002");
    EXPECT_EQ(chatApiGatewayFake.lastRequest.getVerificationMessage(), "ok_msg");
    EXPECT_EQ(chatApiGatewayFake.lastRequest.getRecipientRemark(), "ok_remark");

    const auto local = friendApplicationRepository->of("100000001", "100000002",
                                                       sys::relation::domain::ApplicationStatus::Pending);
    ASSERT_NE(local, nullptr);
    EXPECT_EQ(local->friendApplicationId(), generatedFriendApplicationId);
    EXPECT_EQ(local->verificationMessageValue(), "ok_msg");
    EXPECT_EQ(local->recipientRemarkValue(), "ok_remark");
}

