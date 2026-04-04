//
// Created by 86150 on 2026/3/28.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "sys/user-context/domain/exception/InvalidAvatarException.h"
#include "sys/user-context/domain/exception/InvalidNicknameException.h"
#include "sys/user-context/domain/exception/InvalidPasswordException.h"
#include "sys/user-context/domain/exception/InvalidPhoneException.h"
#include "sys/user-context/domain/service/include/UserValidator.h"
#include "user-context/mock/MockAuthClient.h"
#include "utils/ExceptionTestUtils.h"

class UserValidatorTest : public ::testing::Test
{
};

// rule:
// 用户昵称数量[1,10]，且只能包含中英文和下划线
// 手机号必须是11位数字
// 密码必须至少6位，且只能包含数字、英文和下划线

TEST_F(UserValidatorTest, 当头像为空时_调用validate方法_抛出异常)
{
    testing::StrictMock<tests::user::mock::AuthClientMock> authClient;
    sys::user::domain::UserValidator validator(&authClient);

    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidAvatarException>(
        [&]
        {
            validator.validate("有效昵称", "13800138000", "abc_123", QByteArray());
        },
        "头像不能为空"
    );
}

TEST_F(UserValidatorTest, 当昵称格式不正确时_调用validate方法_抛出异常)
{
    tests::user::mock::AuthClientMock authClient;
    sys::user::domain::UserValidator validator(&authClient);

    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidNicknameException>(
        [&]
        {
            validator.validate("", "13800138000", "abc_123", QByteArray("avatar"));
        },
        "昵称不符合规范"
    );
}

TEST_F(UserValidatorTest, 当手机号格式不正确时_调用validate方法_抛出异常)
{
    testing::StrictMock<tests::user::mock::AuthClientMock> authClient;
    sys::user::domain::UserValidator validator(&authClient);

    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidPhoneException>(
        [&]
        {
            validator.validate("有效昵称", "1380013800a", "abc_123", QByteArray("avatar"));
        },
        "手机号格式不正确"
    );
}

TEST_F(UserValidatorTest, 当密码格式不正确时_调用validate方法_抛出异常)
{
    testing::StrictMock<tests::user::mock::AuthClientMock> authClient;
    sys::user::domain::UserValidator validator(&authClient);

    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("bad-123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{false, ""}));

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidPasswordException>(
        [&]
        {
            validator.validate("有效昵称", "13800138000", "bad-123", QByteArray("avatar"));
        },
        "密码格式不正确"
    );
}

TEST_F(UserValidatorTest, 当输入均正确时_调用validate方法_返回密码的哈希值)
{
    testing::StrictMock<tests::user::mock::AuthClientMock> authClient;
    sys::user::domain::UserValidator validator(&authClient);

    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("abc_123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{true, "HASH_abc_123"}));

    const QString hashedPassword = validator.validate("有效昵称", "13800138000", "abc_123", QByteArray("avatar"));
    EXPECT_EQ(hashedPassword, "HASH_abc_123");
}
