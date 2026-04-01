//
// Created by 86150 on 2026/4/1.
//
#include "UserApplicationServiceTestFixture.h"
//----------------------- 批量获取用户 ----------------------------
// rule:
// 获取用户时，如果本地仓储有用户数据应该直接返回，否则从后端系统获取用户数据
// 如果从后端系统获取了用户数据，本地仓储应该新增用户数据
TEST_F(UserApplicationServiceTestFixture, 批量获取用户时_如果用户ID列表为空_应该返回一个空的用户信息哈希表)
{
    auto users = applicationService->getUserInfos(QSet<QString>{});
    EXPECT_TRUE(users.isEmpty());
}

TEST_F(UserApplicationServiceTestFixture, 批量获取用户时_如果本地仓储有全量用户数据_应该直接返回本地仓储中的用户数据)
{
    // 设置本地仓储数据
    userRepository->save(sys::user::domain::User::of("100000001", "oldAlice", "15011111111", "avatar1", "fileId-123"));
    userRepository->save(sys::user::domain::User::of("100000002", "Bob", "15022222222", "avatar2", "fileId-456"));
    // 设置后端系统数据（不应该被调用到）
    apiGatewayFake->setRemoteUser("100000001", "newAlice", "fileId-123", "15011111111", "");
    apiGatewayFake->setRemoteUser("100000001", "newBob", "fileId-456", "15022222222", "");

    auto users = applicationService->getUserInfos({"100000001", "100000002"});

    // 验证结果
    EXPECT_EQ(users.size(), 2);
    const auto expectedHash = QHash<QString, sys::user::application::UserApplicationService::UserInfo>{
        {"100000001", sys::user::application::UserApplicationService::UserInfo{"100000001", "oldAlice", "fileId-123"}},
        {"100000002", sys::user::application::UserApplicationService::UserInfo{"100000002", "Bob", "fileId-456"}}
    };
    EXPECT_EQ(users, expectedHash);
}

TEST_F(UserApplicationServiceTestFixture, 批量获取用户时_如果本地仓储只有部分用户数据_应该从后端系统获取缺失的用户数据_本地仓储应该新增缺失的用户数据)
{
    // 设置本地仓储数据
    userRepository->save(sys::user::domain::User::of("100000001", "Alice", "15011111111", "avatar1", "fileId-123"));
    // 设置后端系统数据
    apiGatewayFake->setRemoteUser("100000001", "Alice", "fileId-123", "15011111111", "");
    apiGatewayFake->setRemoteUser("100000002", "Bob", "fileId-456", "15022222222", "Bob的个性签名");

    auto users = applicationService->getUserInfos({"100000001", "100000002"});

    // 验证QHash
    EXPECT_EQ(users.size(), 2);
    const auto expectedHash = QHash<QString, sys::user::application::UserApplicationService::UserInfo>{
        {"100000001", sys::user::application::UserApplicationService::UserInfo{"100000001", "Alice", "fileId-123"}},
        {"100000002", sys::user::application::UserApplicationService::UserInfo{"100000002", "Bob", "fileId-456"}}
    };
    EXPECT_EQ(users, expectedHash);
    // 验证本地仓储新增了缺失的用户数据
    auto newUser = userRepository->of("100000002");
    ASSERT_NE(newUser, nullptr);
    ASSERT_EQ(newUser->userId(), "100000002");
    ASSERT_EQ(newUser->nicknameValue(), "Bob");
    ASSERT_EQ(newUser->avatarFileId(), "fileId-456");
    ASSERT_EQ(newUser->phoneValue(), "15022222222");
    ASSERT_EQ(newUser->descriptionValue(), "Bob的个性签名");
}

TEST_F(UserApplicationServiceTestFixture, 批量获取用户时_如果部分用户数据本地仓储和后端系统都没有_应该返回一个不包含这些用户数据的哈希表)
{
    // 设置本地仓储数据
    userRepository->save(sys::user::domain::User::of("100000001", "Alice", "15011111111", "avatar1", "fileId-123"));
    // 设置后端系统数据
    apiGatewayFake->setRemoteUser("100000001", "Alice", "fileId-123", "15011111111", "");

    auto users = applicationService->getUserInfos({"100000001", "100000002"});

    // 验证QHash
    EXPECT_EQ(users.size(), 1);
    const auto expectedHash = QHash<QString, sys::user::application::UserApplicationService::UserInfo>{
        {"100000001", sys::user::application::UserApplicationService::UserInfo{"100000001", "Alice", "fileId-123"}},
    };
    EXPECT_EQ(users, expectedHash);

    // 验证本地仓储没有新增不存在的用户数据
    auto newUser = userRepository->of("100000002");
    EXPECT_EQ(newUser, nullptr);
}
