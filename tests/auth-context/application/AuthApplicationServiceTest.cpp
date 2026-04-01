//
// Created by 86150 on 2026/3/29.
//

#include <gtest/gtest.h>

#include "auth-context/application/fake/AuthApiGatewayFake.h"
#include "contract/system-provider/auth-context-provider/Login.h"
#include "sys/auth-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/auth-context/application/service/include/AuthApplicationService.h"
#include "sys/auth-context/domain/object/include/UserCredential.h"
#include "sys/auth-context/domain/service/include/UserCredentialService.h"
#include "sys/common/component/UserCredentialManager.h"

class AuthApplicationServiceTest : public testing::Test
{
protected:
    void SetUp() override
    {
        sys::common::component::UserCredentialManager::instance().update("", "");
        backendClient = std::make_unique<sys::auth::adapter::BackendClientAdapter>(&apiGatewayFake);
        userCredentialService = std::make_unique<sys::auth::domain::UserCredentialService>(backendClient.get());
        authApplicationService = std::make_unique<sys::auth::application::AuthApplicationService>(
            userCredentialService.get()
        );
    }

    contract::auth::LoginRequest buildValidRequest() const
    {
        contract::auth::LoginRequest request;
        const auto& fakeUser = apiGatewayFake.defaultUser();
        request.account = fakeUser.account;
        request.password = fakeUser.password;
        return request;
    }

protected:
    tests::auth::fake::AuthApiGatewayFake apiGatewayFake;
    std::unique_ptr<sys::auth::adapter::BackendClientAdapter> backendClient;
    std::unique_ptr<sys::auth::domain::UserCredentialService> userCredentialService;
    std::unique_ptr<sys::auth::application::AuthApplicationService> authApplicationService;
};

// rule:
// 账号要么是手机号纯9位数字的用户ID但不允许以0开头
TEST_F(AuthApplicationServiceTest, 登录时_如果密码格式不正确_应该给出提示)
{
    auto request = buildValidRequest();
    request.password = "bad-123";

    const auto response = authApplicationService->login(request);

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "密码格式不正确");
    EXPECT_EQ(apiGatewayFake.loginCallCount, 0);
}

TEST_F(AuthApplicationServiceTest, 登录时_如果账号密码不匹配_应该给出提示)
{
    auto request = buildValidRequest();
    request.password = "abc_124";

    const auto response = authApplicationService->login(request);

    EXPECT_FALSE(response.success);
    EXPECT_TRUE(response.errMsg.has_value());
    EXPECT_EQ(response.errMsg.value(), "账号或密码错误");
    EXPECT_EQ(apiGatewayFake.loginCallCount, 1);
}

TEST_F(AuthApplicationServiceTest, 登录时_如果密码格式正确_传给后端的密码应该是哈希后的值)
{
    const auto request = buildValidRequest();

    const auto response = authApplicationService->login(request);

    EXPECT_TRUE(response.success);
    ASSERT_TRUE(apiGatewayFake.lastRequest.has_value());
    EXPECT_EQ(apiGatewayFake.lastRequest->getAccount(), request.account);
    EXPECT_EQ(
        apiGatewayFake.lastRequest->getPassword(),
        sys::auth::domain::Password::of(request.password).hashedValue()
    );
}

TEST_F(AuthApplicationServiceTest, 登录时_如果账号密码正确_应该成功登录并缓存凭据)
{
    const auto request = buildValidRequest();
    const auto& fakeUser = apiGatewayFake.defaultUser();

    const auto response = authApplicationService->login(request);

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.userId, fakeUser.userId);
    EXPECT_EQ(apiGatewayFake.loginCallCount, 1);
    EXPECT_EQ(sys::common::component::UserCredentialManager::instance().getCurrentUserId(), fakeUser.userId);
    EXPECT_EQ(sys::common::component::UserCredentialManager::instance().getCurrentToken(), fakeUser.token);
}
