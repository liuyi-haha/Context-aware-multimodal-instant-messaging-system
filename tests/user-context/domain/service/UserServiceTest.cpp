//
// Created by 86150 on 2026/3/28.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "sys/user-context/domain/exception/InvalidAvatarException.h"
#include "sys/user-context/domain/exception/InvalidNicknameException.h"
#include "sys/user-context/domain/exception/InvalidPasswordException.h"
#include "sys/user-context/domain/exception/InvalidPhoneException.h"
#include "sys/user-context/domain/exception/PhoneHasBeenRegistered.h"
#include "sys/user-context/domain/exception/InvalidAccountException.h"
#include "sys/user-context/domain/exception/UserNotFoundException.h"
#include "sys/user-context/domain/service/include/UserService.h"
#include "sys/user-context/domain/service/include/UserValidator.h"
#include "user-context/mock/MockAuthClient.h"
#include "user-context/mock/MockBackendClient.h"
#include "user-context/mock/MockFileClient.h"
#include "user-context/mock/MockUserRepository.h"
#include "utils/ExceptionTestUtils.h"

class UserServiceTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        userValidator = std::make_unique<sys::user::domain::UserValidator>(&authClient);
        userService = std::make_unique<sys::user::domain::UserService>(
            userValidator.get(),
            &backendClient,
            &fileClient,
            &userRepository
        );
    }

protected:
    testing::StrictMock<tests::user::mock::AuthClientMock> authClient;
    testing::StrictMock<tests::user::mock::BackendClientMock> backendClient;
    testing::StrictMock<tests::user::mock::FileClientMock> fileClient;
    testing::StrictMock<tests::user::mock::UserRepositoryMock> userRepository;

    std::unique_ptr<sys::user::domain::UserValidator> userValidator;
    std::unique_ptr<sys::user::domain::UserService> userService;
};

// rule:
// 用户必须上传头像
// 用户昵称必须大于1个字符，不大于10个字符，且只能包含中英文和下划线
// 用户手机号必须是11位数字
// 用户密码必须至少6位，且只能包含数字、英文和下划线
// 手机号不能重复注册
TEST_F(UserServiceTest, 当头像为空时_调用registerUser方法_抛出异常)
{
    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);
    EXPECT_CALL(backendClient, registerUser(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);
    EXPECT_CALL(userRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidAvatarException>(
        [&]
        {
            userService->registerUser("有效昵称", "13800138000", "abc_123", QByteArray());
        },
        "头像不能为空"
    );
}

TEST_F(UserServiceTest, 当昵称格式不正确时_调用registerUser方法_抛出异常)
{
    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);
    EXPECT_CALL(backendClient, registerUser(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);
    EXPECT_CALL(userRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidNicknameException>(
        [&]
        {
            userService->registerUser("a", "13800138000", "abc_123", QByteArray("avatar"));
        },
        "昵称不符合规范"
    );
}

TEST_F(UserServiceTest, 当手机号格式不正确时_调用registerUser方法_抛出异常)
{
    EXPECT_CALL(authClient, validateAndHashPassword(testing::_)).Times(0);
    EXPECT_CALL(backendClient, registerUser(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);
    EXPECT_CALL(userRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidPhoneException>(
        [&]
        {
            userService->registerUser("有效昵称", "1380013800a", "abc_123", QByteArray("avatar"));
        },
        "手机号格式不正确"
    );
}

TEST_F(UserServiceTest, 当密码格式不正确时_调用registerUser方法_抛出异常)
{
    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("bad-123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{false, ""}));
    EXPECT_CALL(backendClient, registerUser(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);
    EXPECT_CALL(userRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidPasswordException>(
        [&]
        {
            userService->registerUser("有效昵称", "13800138000", "bad-123", QByteArray("avatar"));
        },
        "密码格式不正确"
    );
}

TEST_F(UserServiceTest, 当输入合法时_调用registerUser方法_成功注册用户并返回用户ID)
{
    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("abc_123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{true, "HASH_abc_123"}));

    EXPECT_CALL(backendClient,
                registerUser(QStringLiteral("HASH_abc_123"), QStringLiteral("有效昵称"), QStringLiteral("13800138000")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::BackendClient::RegisterUserResult{"123456789", "A10001"}));

    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(1);
    EXPECT_CALL(userRepository, save(testing::_)).Times(1);

    const QString userId = userService->registerUser(
        "有效昵称",
        "13800138000",
        "abc_123",
        QByteArray("avatar")
    );

    EXPECT_EQ(userId, "123456789");
}

TEST_F(UserServiceTest, 当手机号已被注册时_调用registerUser方法_抛出异常)
{
    EXPECT_CALL(authClient, validateAndHashPassword(QStringLiteral("abc_123")))
        .Times(1)
        .WillOnce(testing::Return(sys::user::port::AuthResult{true, "HASH_abc_123"}));

    EXPECT_CALL(backendClient,
                registerUser(QStringLiteral("HASH_abc_123"), QStringLiteral("有效昵称"), QStringLiteral("13800138000")))
        .Times(1)
        .WillOnce(testing::Throw(sys::user::domain::PhoneHasBeenRegisteredException()));

    EXPECT_CALL(fileClient, uploadAvatar(testing::_)).Times(0);
    EXPECT_CALL(userRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::PhoneHasBeenRegisteredException>(
        [&]
        {
            userService->registerUser(
                "有效昵称",
                "13800138000",
                "abc_123",
                QByteArray("avatar")
            );
        },
        "手机号已被注册"
    );
}


// rule:
// 用户输入的账号要么是11位的手机号，要么是9位的用户ID
TEST_F(UserServiceTest, 当账号不合法时_调用searchUser方法_抛出异常)
{
    EXPECT_CALL(backendClient, searchUser(testing::_)).Times(0);
    EXPECT_CALL(userRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::InvalidAccountException>(
        [&]
        {
            userService->searchUser("abc");
        },
        "账号格式不正确，应为手机号或用户ID"
    );
}

TEST_F(UserServiceTest, 当用户不存在时_调用searchUser方法_抛出异常)
{
    EXPECT_CALL(backendClient, searchUser(QStringLiteral("13800138000")))
        .Times(1)
        .WillOnce(testing::Throw(sys::user::domain::UserNotFoundException()));
    EXPECT_CALL(userRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::user::domain::UserNotFoundException>(
        [&]
        {
            userService->searchUser("13800138000");
        },
        "用户不存在"
    );
}

