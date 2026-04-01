//
// Created by 86150 on 2026/4/1.
//
#include "UserApplicationServiceTestFixture.h"

//----------------------- 搜索用户 ----------------------------
// rule:
// 账号要么是手机号纯9位数字的用户ID但不允许以0开头
// 搜索用户时，应当获取后端系统的最新用户信息
// 搜索用户时，如果后端系统有用户数据，本地仓储应该新增或更新用户数据
// 如果没有搜索到用户, 属于正常情况，因为用户可能输入了一个不存在的账号
TEST_F(UserApplicationServiceTestFixture, 搜索用户时_如果账号不合法_应该给出提示)
{
    const auto response = applicationService->searchUser(buildSearchRequest("abc"));

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "账号格式不正确，应为手机号或用户ID");
    EXPECT_EQ(apiGatewayFake->searchUserCallCount, 0);
}

TEST_F(UserApplicationServiceTestFixture, 搜索用户时_如果账号格式合法但用户不存在_结果应该成功但没有用户数据)
{
    const auto response = applicationService->searchUser(buildSearchRequest("987654321"));

    EXPECT_TRUE(response.success);
    EXPECT_FALSE(response.userView.has_value());
    EXPECT_EQ(apiGatewayFake->searchUserCallCount, 1);
    EXPECT_EQ(apiGatewayFake->lastSearchKeyword, "987654321");
}

TEST_F(UserApplicationServiceTestFixture, 搜索用户时_如果本地和后端都有用户数据时_结果应该是后端的用户数据_本地仓储应该更新用户数据)
{
    const auto oldLocal = sys::user::domain::User::of(
        "123456789", "本地旧昵称", "13800138001", QByteArray(), "local-avatar", "");
    userRepository->save(oldLocal);

    apiGatewayFake->setRemoteUser("123456789", "后端新昵称", "remote-avatar", "13800138001");

    const auto response = applicationService->searchUser(buildSearchRequest("123456789"));

    EXPECT_TRUE(response.success);
    EXPECT_TRUE(response.userView.has_value());
    EXPECT_EQ(response.userView->userId, "123456789");
    EXPECT_EQ(response.userView->nickname, "后端新昵称");
    EXPECT_EQ(response.userView->avatarFileId, "remote-avatar");

    const auto localUser = userRepository->of("123456789");
    ASSERT_NE(localUser, nullptr);
    EXPECT_EQ(localUser->nicknameValue(), "后端新昵称");
    EXPECT_EQ(localUser->avatarFileId(), "remote-avatar");
}

TEST_F(UserApplicationServiceTestFixture, 搜索用户时_如果从后端获取到用户数据且本地没有用户数据时_本地仓储应该新增用户数据)
{
    apiGatewayFake->setRemoteUser("234567891", "后端新增用户", "avatar-new", "18800112233");

    EXPECT_EQ(userRepository->of("234567891"), nullptr);

    const auto response = applicationService->searchUser(buildSearchRequest("18800112233"));

    EXPECT_TRUE(response.success);
    EXPECT_TRUE(response.userView.has_value());
    EXPECT_EQ(response.userView->userId, "234567891");

    const auto localUser = userRepository->of("234567891");
    ASSERT_NE(localUser, nullptr);
    EXPECT_EQ(localUser->nicknameValue(), "后端新增用户");
    EXPECT_EQ(localUser->avatarFileId(), "avatar-new");
}
