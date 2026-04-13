#include "UserApplicationServiceTestFixture.h"
//
// Created by 86150 on 2026/4/1.
//
// rule:
// 用户昵称数量[1,10]，且只能包含中英文和下划线
// 手机号必须是11位数字
// 密码必须至少6位，且只能包含数字、英文和下划线
// 手机号不能被重复注册
TEST_F(UserApplicationServiceTestFixture, 注册用户时_如果昵称不合法_应该给出提示)
{
    auto request = buildValidRegisterUserRequest();
    request.nickname = "";

    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);

    const auto response = applicationService->registerUser(request);

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "昵称不符合规范");
    EXPECT_EQ(apiGatewayFake->registerUserCallCount, 0);
}

TEST_F(UserApplicationServiceTestFixture, 注册用户时_如果手机号不合法_应该给出提示)
{
    auto request = buildValidRegisterUserRequest();
    request.phone = "1380013800a";

    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);

    const auto response = applicationService->registerUser(request);

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "手机号格式不正确");
    EXPECT_EQ(apiGatewayFake->registerUserCallCount, 0);
}

TEST_F(UserApplicationServiceTestFixture, 注册用户时_如果密码不合法_应该给出提示)
{
    auto request = buildValidRegisterUserRequest();
    request.password = "bad-123";

    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("bad-123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{false, ""}));
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);

    const auto response = applicationService->registerUser(request);

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "密码格式不正确");
    EXPECT_EQ(apiGatewayFake->registerUserCallCount, 0);
}

TEST_F(UserApplicationServiceTestFixture, 注册用户时_如果没有上传头像_应该给出提示)
{
    auto request = buildValidRegisterUserRequest();
    request.avatarFileInfo = QFileInfo(); // 设置为无效的 QFileInfo，模拟未上传头像

    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);

    const auto response = applicationService->registerUser(request);

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "头像不能为空");
    EXPECT_EQ(apiGatewayFake->registerUserCallCount, 0);
}

TEST_F(UserApplicationServiceTestFixture, 注册用户时_如果输入的手机号已经被注册_应该给出提示)
{
    auto request = buildValidRegisterUserRequest();
    request.phone = "13800138000";

    apiGatewayFake->upsertRemoteUser(
        tests::user::fake::UserServiceApiGatewayFake::buildProfile(
            "223456781", "13800138000", "已注册用户甲", "avatar-seed-1", "seed-user-1"));

    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("abc_123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{true, "HASH_abc_123"}));

    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);

    const auto response = applicationService->registerUser(request);

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "手机号已被注册");
    EXPECT_EQ(apiGatewayFake->registerUserCallCount, 1);
}

TEST_F(UserApplicationServiceTestFixture, 注册用户时_如果输入合法_应该返回userId)
{
    auto request = buildValidRegisterUserRequest();

    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("abc_123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{true, "HASH_abc_123"}));

    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(1);

    const auto response = applicationService->registerUser(request);

    EXPECT_TRUE(response.success);
    EXPECT_TRUE(response.userId.has_value());
    EXPECT_EQ(response.userId.value(), "123456789");
    EXPECT_EQ(apiGatewayFake->registerUserCallCount, 1);
}

TEST_F(UserApplicationServiceTestFixture, 注册用户时_如果输入合法_应该注册成功并且用户在本地被保存)
{
    auto request = buildValidRegisterUserRequest();

    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("abc_123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{true, "HASH_abc_123"}));

    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(1);

    const auto response = applicationService->registerUser(request);

    EXPECT_TRUE(response.success);
    EXPECT_TRUE(response.userId.has_value());
    EXPECT_EQ(response.userId.value(), "123456789");

    // 按要求通过 of 接口验证本地仓储中已保存用户
    const auto localUser = userRepository->of(response.userId.value());
    EXPECT_NE(localUser, nullptr);
    EXPECT_EQ(localUser->userId(), "123456789");
    EXPECT_EQ(apiGatewayFake->registerUserCallCount, 1);
}

