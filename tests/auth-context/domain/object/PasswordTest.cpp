#include <gtest/gtest.h>
#include <vector>

#include "sys/auth-context/domain/exception/InvalidPasswordException.h"
#include "sys/auth-context/domain/object/include/UserCredential.h"
#include "utils/ExceptionTestUtils.h"

class PasswordTest : public testing::Test
{
};

// rule:
// 用户密码必须至少6位，且只能包含数字、英文和下划线
// 账号要么是手机号纯9位数字的用户ID但不允许以0开头
TEST_F(PasswordTest, 当密码格式不正确时_调用静态check方法_应该返回false)
{
    const std::vector<QString> invalidPasswords = {
        "",
        "12345",
        "abc-123",
        "abc 123",
        "abc@123",
        "abc中文12"
    };

    for (const auto& password : invalidPasswords)
    {
        EXPECT_FALSE(sys::auth::domain::Password::checkPassword(password));
    }
}

TEST_F(PasswordTest, 当密码格式不正确时_创建Password对象_应该抛出异常)
{
    tests::utils::expectThrowWithMessage<sys::auth::domain::InvalidPasswordException>(
        []
        {
            (void)sys::auth::domain::Password::of("bad-123");
        },
        "密码格式不正确"
    );
}

TEST_F(PasswordTest, 当密码格式正确时_调用静态check方法_应该返回true)
{
    const std::vector<QString> validPasswords = {
        "abc123",
        "abc_12",
        "A1_b2C3",
        "123456"
    };

    for (const auto& password : validPasswords)
    {
        EXPECT_TRUE(sys::auth::domain::Password::checkPassword(password));
    }
}

TEST_F(PasswordTest, 当密码格式正确时_创建Password对象_应该成功创建)
{
    const auto password = sys::auth::domain::Password::of("abc_123");
    EXPECT_EQ(password.value(), "abc_123");
}


TEST_F(PasswordTest, 当Password对象正常创建时_调用hashedValue方法_应该返回正确的哈希值)
{
    const auto password = sys::auth::domain::Password::of("abc_123");

    EXPECT_NE(password.hashedValue(), "abc_123");
}


