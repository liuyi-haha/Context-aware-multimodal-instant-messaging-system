//
// Created by 86150 on 2026/3/29.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "auth-context/mock/MockBackendClient.h"
#include "sys/auth-context/domain/exception/AccountPasswordNotMatchedException.h"
#include "sys/auth-context/domain/exception/InvalidPasswordException.h"
#include "sys/auth-context/domain/service/include/UserCredentialService.h"
#include "sys/auth-context/domain/object/include/UserCredential.h"
#include "sys/common/component/UserCredentialManager.h"
#include "utils/ExceptionTestUtils.h"

class UserCredentialServiceTest : public testing::Test
{
protected:
    void SetUp() override
    {
        sys::common::component::UserCredentialManager::instance().update("", "");
        userCredentialService = std::make_unique<sys::auth::domain::UserCredentialService>(&backendClient);
    }

protected:
    testing::StrictMock<tests::auth::mock::BackendClientMock> backendClient;
    std::unique_ptr<sys::auth::domain::UserCredentialService> userCredentialService;
};

TEST_F(UserCredentialServiceTest, 当用户输入的密码格式不正确时_调用login方法_应该抛出异常)
{
    EXPECT_CALL(backendClient, login(testing::_, testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::auth::domain::InvalidPasswordException>(
        [&]
        {
            userCredentialService->login("13800138000", "bad-123");
        },
        "密码格式不正确"
    );
}

TEST_F(UserCredentialServiceTest, 当账号和密码不匹配时_调用login方法_应该抛出异常)
{
    const QString hashedPassword = sys::auth::domain::Password::of("abc_123").hashedValue();

    EXPECT_CALL(backendClient, login(QStringLiteral("13800138000"), hashedPassword))
        .Times(1)
        .WillOnce(testing::Throw(sys::auth::domain::AccountPasswordNotMatchedException()));

    tests::utils::expectThrowWithMessage<sys::auth::domain::AccountPasswordNotMatchedException>(
        [&]
        {
            userCredentialService->login("13800138000", "abc_123");
        },
        "账号或密码错误"
    );
}

TEST_F(UserCredentialServiceTest, 当账号密码正确时_调用login方法_应该返回用户ID并缓存凭据)
{
    const QString hashedPassword = sys::auth::domain::Password::of("abc_123").hashedValue();

    EXPECT_CALL(backendClient, login(QStringLiteral("13800138000"), hashedPassword))
        .Times(1)
        .WillOnce(testing::Return(sys::auth::port::BackendClient::LoginResult{"123456789", "token-001"}));

    const QString userId = userCredentialService->login("13800138000", "abc_123");

    EXPECT_EQ(userId, "123456789");
    EXPECT_EQ(sys::common::component::UserCredentialManager::instance().getCurrentUserId(), "123456789");
    EXPECT_EQ(sys::common::component::UserCredentialManager::instance().getCurrentToken(), "token-001");
}
