//
// Created by 86150 on 2026/3/29.
//
#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include "relation-context/mock/MockFriendApplicationRepository.h"
#include "relation-context/mock/MockFriendShipRepository.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/exception/FriendApplicationAlreadyProcessedException.h"
#include "sys/relation-context/domain/exception/FriendApplicationHandleUnauthorizedException.h"
#include "sys/relation-context/domain/exception/FriendRelationAlreadyExistsException.h"
#include "sys/relation-context/domain/exception/InvalidRecipientRemarkException.h"
#include "sys/relation-context/domain/exception/InvalidTargetUserIdException.h"
#include "sys/relation-context/domain/exception/InvalidVerificationMessageException.h"
#include "sys/relation-context/domain/exception/PendingFriendApplicationAlreadyExistsException.h"
#include "sys/relation-context/domain/service/include/FriendApplicationValidator.h"
#include "utils/ExceptionTestUtils.h"

class FriendApplicationValidatorTest : public testing::Test
{
protected:
    void SetUp() override
    {
        sys::common::component::UserCredentialManager::instance().update("100000001", "token");
        validator = std::make_unique<sys::relation::domain::FriendApplicationValidator>(
            &friendApplicationRepository,
            &friendShipRepository);
    }

    void TearDown() override
    {
        sys::common::component::UserCredentialManager::instance().update("", "");
    }

    static QSharedPointer<sys::relation::domain::FriendApplication> makePending()
    {
        return sys::relation::domain::FriendApplication::of(
            "100000001", "FA-001", "100000002", "msg_a", "remark_a", QDateTime::currentDateTime());
    }

    static QSharedPointer<sys::relation::domain::FriendApplication> makeAccepted()
    {
        auto app = makePending();
        app->accept();
        return app;
    }

    static QSharedPointer<sys::relation::domain::FriendApplication> makeRejected()
    {
        auto app = makePending();
        app->reject();
        return app;
    }

protected:
    testing::StrictMock<tests::common::mock::MockFriendApplicationRepository> friendApplicationRepository;
    testing::StrictMock<tests::common::mock::MockFriendShipRepository> friendShipRepository;
    std::unique_ptr<sys::relation::domain::FriendApplicationValidator> validator;
};

//---------------------- validateSendFriendApplicationInputs--------------------------
// rule:
// 好友申请的验证消息长度限制为[1, 20]，字符类型不限
// 好友申请的对方备注长度限制为[1, 20]，字符类型不限
// 申请用户不能在好友申请未被处理前，再次向同一用户发送好友申请
// 申请用户和目标用户不能已经是好友关系，否则不允许发送好友申请
// 申请用户和目标用户不能是同一个人
TEST_F(FriendApplicationValidatorTest, 当验证消息长度不合法时_调用validateSendFriendApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::InvalidVerificationMessageException>(
        [&]
        {
            validator->validateSendFriendApplicationInputs("aaaaaaaaaaaaaaaaaaaaa", "ok_remark", "100000002");
        },
        "验证消息长度不合法");
}


TEST_F(FriendApplicationValidatorTest, 当对方备注长度不合法时_调用validateSendFriendApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::InvalidRecipientRemarkException>(
        [&]
        {
            validator->validateSendFriendApplicationInputs("ok_msg", "bbbbbbbbbbbbbbbbbbbbb", "100000002");
        },
        "对方备注长度不合法");

    tests::utils::expectThrowWithMessage<sys::relation::domain::InvalidRecipientRemarkException>(
        [&]
        {
            validator->validateSendFriendApplicationInputs("ok_msg", "", "100000002");
        },
        "对方备注长度不合法");
}

TEST_F(FriendApplicationValidatorTest, 当存在发送给目标用户且状态为Pending状态的好友申请时_调用validateSendFriendApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(makePending()));
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::PendingFriendApplicationAlreadyExistsException>(
        [&]
        {
            validator->validateSendFriendApplicationInputs("ok_msg", "ok_remark", "100000002");
        },
        "已存在待处理的好友申请");
}

TEST_F(FriendApplicationValidatorTest, 当申请用户和目标用户已经是好友时_调用validateSendFriendApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(friendShipRepository, of(QStringLiteral("100000001"), QStringLiteral("100000002")))
        .Times(1)
        .WillOnce(testing::Return(sys::relation::domain::FriendShip::of("FS-1", "100000001", "100000002")));

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendShipAlreadyExistsException>(
        [&]
        {
            validator->validateSendFriendApplicationInputs("ok_msg", "ok_remark", "100000002");
        },
        "双方已经是好友");
}

TEST_F(FriendApplicationValidatorTest, 当目标用户就是申请用户时_调用validateSendFriendApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::ApplyFriendToMyselfException>(
        [&]
        {
            validator->validateSendFriendApplicationInputs("ok_msg", "ok_remark", "100000001");
        },
        "不能添加自己为好友");
}

TEST_F(FriendApplicationValidatorTest, 当验证消息长度为0时_调用validateSendFriendApplicationInputs_应该通过)
{
    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(friendShipRepository, of(QStringLiteral("100000001"), QStringLiteral("100000002")))
        .Times(1)
        .WillOnce(testing::Return(nullptr));

    EXPECT_NO_THROW(
        validator->validateSendFriendApplicationInputs("", "ok_remark", "100000002"));
}

TEST_F(FriendApplicationValidatorTest, 当验证消息长度为20时_调用validateSendFriendApplicationInputs_应该通过)
{
    const QString message20 = QStringLiteral("aaaaaaaaaaaaaaaaaaaa");

    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(friendShipRepository, of(QStringLiteral("100000001"), QStringLiteral("100000002")))
        .Times(1)
        .WillOnce(testing::Return(nullptr));

    EXPECT_NO_THROW(
        validator->validateSendFriendApplicationInputs(message20, "ok_remark", "100000002"));
}

TEST_F(FriendApplicationValidatorTest, 当对方备注长度为1时_调用validateSendFriendApplicationInputs_应该通过)
{
    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(friendShipRepository, of(QStringLiteral("100000001"), QStringLiteral("100000002")))
        .Times(1)
        .WillOnce(testing::Return(nullptr));

    EXPECT_NO_THROW(
        validator->validateSendFriendApplicationInputs("ok_msg", "1", "100000002"));
}

TEST_F(FriendApplicationValidatorTest, 当对方备注长度为20时_调用validateSendFriendApplicationInputs_应该通过)
{
    const QString remark20 = QStringLiteral("bbbbbbbbbbbbbbbbbbbb");

    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(friendShipRepository, of(QStringLiteral("100000001"), QStringLiteral("100000002")))
        .Times(1)
        .WillOnce(testing::Return(nullptr));

    EXPECT_NO_THROW(
        validator->validateSendFriendApplicationInputs("ok_msg", remark20, "100000002"));
}


//---------------------- validateHandleApplicationInputs--------------------------
// rule:
// 已经处理过的好友申请不能再被处理
// 只有好友申请的目标用户是自己时，才允许处理好友申请
TEST_F(FriendApplicationValidatorTest, 当好友申请状态为Accepted时_调用validateHandleApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(QStringLiteral("FA-001")))
        .Times(1)
        .WillOnce(testing::Return(makeAccepted()));

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendApplicationAlreadyProcessedException>(
        [&]
        {
            validator->validateHandleFriendApplicationInputs("FA-001", "100000002");
        },
        "好友申请已被处理");
}

TEST_F(FriendApplicationValidatorTest, 当好友申请状态为Rejected时_调用validateHandleApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(QStringLiteral("FA-001")))
        .Times(1)
        .WillOnce(testing::Return(makeRejected()));

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendApplicationAlreadyProcessedException>(
        [&]
        {
            validator->validateHandleFriendApplicationInputs("FA-001", "100000002");
        },
        "好友申请已被处理");
}

TEST_F(FriendApplicationValidatorTest, 当好友申请的目标用户不是当前用户时_调用validateHandleApplicationInputs_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(QStringLiteral("FA-001")))
        .Times(1)
        .WillOnce(testing::Return(makePending()));

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendApplicationHandleUnauthorizedException>(
        [&]
        {
            validator->validateHandleFriendApplicationInputs("FA-001", "100000003");
        },
        "无权处理该好友申请");
}
