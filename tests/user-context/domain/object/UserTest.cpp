#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

#include "sys/user-context/domain/exception/InvalidNicknameException.h"
#include "sys/user-context/domain/exception/InvalidPhoneException.h"
#include "sys/user-context/domain/object/include/User.h"
#include "utils/ExceptionTestUtils.h"

class UserTest : public ::testing::Test
{
};

// rule:
// 用户昵称数量[1,10]，且只能包含中英文和下划线
// 用户手机号必须是11位数字
TEST_F(UserTest, 昵称格式正确时_调用静态check方法_返回true)
{
    const std::vector<QString> validNicknames = {
        "ab", // 最小合法长度 2
        "abcd_efghi" // 最大合法长度 10
    };

    for (const auto& nickname : validNicknames)
    {
        EXPECT_TRUE(sys::user::domain::Nickname::checkNickname(nickname));
    }
}

TEST_F(UserTest, 昵称格式不正确时_调用静态check方法_返回false)
{
    const std::vector<QString> invalidNicknames = {
        "",
        "abcdefghijk", // 11
        "nick-name",
        "name 1",
        "@昵称"
    };

    for (const auto& nickname : invalidNicknames)
    {
        EXPECT_FALSE(sys::user::domain::Nickname::checkNickname(nickname));
    }
}

TEST_F(UserTest, 昵称格式不正确时_构造User_抛出异常)
{
    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidNicknameException>(
        []
        {
            sys::user::domain::User::of(
                "123456789",
                "bad name",
                "13800138000",
                QByteArray(),
                "avatar-file-id"
            );
        },
        "昵称不符合规范"
    );
}

// rule: 手机号必须是11位数字
TEST_F(UserTest, 手机号格式正确时_调用静态check方法_返回true)
{
    const std::vector<QString> validPhones = {
        "13800138000",
        "19912345678"
    };

    for (const auto& phone : validPhones)
    {
        EXPECT_TRUE(sys::user::domain::Phone::checkPhone(phone));
    }
}

TEST_F(UserTest, 手机号格式不正确时_调用静态check方法_返回false)
{
    const std::vector<QString> invalidPhones = {
        "",
        "1234567890", // 10
        "123456789012", // 12
        "1380013800a",
        "1380013-000"
    };

    for (const auto& phone : invalidPhones)
    {
        EXPECT_FALSE(sys::user::domain::Phone::checkPhone(phone));
    }
}

TEST_F(UserTest, 手机号格式不正确时_构造User_抛出异常)
{
    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidPhoneException>(
        []
        {
            sys::user::domain::User::of(
                "123456789",
                "有效昵称_abc",
                "1380013800a",
                QByteArray(),
                "avatar-file-id"
            );
        },
        "手机号格式不正确"
    );
}

TEST_F(UserTest, 当账号不正确时_调用静态checkAccount方法_应该返回false)
{
    const std::vector<QString> invalidAccounts = {
        "",
        "abc",
        "012345678", // 9位但以0开头
        "12345678", // 8位数字
        "1234567890", // 10位数字
        "1380013800a", // 手机号包含非数字
        "U12345678" // 历史格式，不再允许
    };

    for (const auto& account : invalidAccounts)
    {
        EXPECT_FALSE(sys::user::domain::User::checkAccount(account));
    }
}
