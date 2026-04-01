//
// Created by 86150 on 2026/4/1.
//
#include "UserApplicationServiceTestFixture.h"
//----------------------- 获取用户 ----------------------------

// rule:
// 账号要么是手机号纯9位数字的用户ID但不允许以0开头
// 获取用户时，如果本地仓储有用户数据应该直接返回，否则从后端系统获取用户数据
// 如果从后端系统获取了用户数据，本地仓储应该新增用户数据
// 如果没有搜索到用户, 属于非正常情况，因为该接口是供UI自动调用的，而非用户触发，所以用户ID应该是存在的
TEST_F(UserApplicationServiceTestFixture, 获取用户时_如果用户ID不合法_应该给出提示)
{
    const auto response = applicationService->getUser(buildGetUserRequest("012345678"));

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "用户ID不合法");
    EXPECT_EQ(apiGatewayFake->searchUserCallCount, 0);
}

TEST_F(UserApplicationServiceTestFixture, 获取用户时_如果本地仓储有用户_应该直接返回本地数据)
{
    const auto localUser = sys::user::domain::User::of(
        "234567891", "本地用户", "18800112233", QByteArray(), "avatar-local");
    userRepository->save(localUser);

    const auto response = applicationService->getUser(buildGetUserRequest("234567891"));

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.userView.userId, "234567891");
    EXPECT_EQ(response.userView.nickname, "本地用户");
    EXPECT_EQ(response.userView.avatarFileId, "avatar-local");
    EXPECT_EQ(apiGatewayFake->searchUserCallCount, 0);
}

TEST_F(UserApplicationServiceTestFixture, 获取用户时_如果本地没有但后端有_应该返回后端数据并保存到本地)
{
    const auto remoteUser = tests::user::fake::UserServiceApiGatewayFake::buildProfile(
        "345678912", "13600001111", "后端用户", "avatar-remote", "remote-desc");
    apiGatewayFake->upsertRemoteUser(remoteUser);

    EXPECT_EQ(userRepository->of("345678912"), nullptr);

    const auto response = applicationService->getUser(buildGetUserRequest("345678912"));

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.userView.userId, "345678912");
    EXPECT_EQ(response.userView.nickname, "后端用户");
    EXPECT_EQ(response.userView.avatarFileId, "avatar-remote");
    EXPECT_EQ(apiGatewayFake->searchUserCallCount, 1);

    const auto savedUser = userRepository->of("345678912");
    ASSERT_NE(savedUser, nullptr);
    EXPECT_EQ(savedUser->nicknameValue(), "后端用户");
    EXPECT_EQ(savedUser->avatarFileId(), "avatar-remote");
}

TEST_F(UserApplicationServiceTestFixture, 获取用户时_如果本地和后端都没有_应该返回失败)
{
    const auto response = applicationService->getUser(buildGetUserRequest("456789123"));

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "用户不存在");
    EXPECT_EQ(apiGatewayFake->searchUserCallCount, 1);
}
