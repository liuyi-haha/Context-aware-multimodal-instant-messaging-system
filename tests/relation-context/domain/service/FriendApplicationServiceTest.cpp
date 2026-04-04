//
// Created by 86150 on 2026/3/29.
//
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "relation-context/mock/MockBackendClient.h"
#include "relation-context/mock/MockFriendApplicationRepository.h"
#include "relation-context/mock/MockFriendShipRepository.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/exception/FriendRelationAlreadyExistsException.h"
#include "sys/relation-context/domain/exception/InvalidRecipientRemarkException.h"
#include "sys/relation-context/domain/exception/InvalidTargetUserIdException.h"
#include "sys/relation-context/domain/exception/InvalidVerificationMessageException.h"
#include "sys/relation-context/domain/exception/PendingFriendApplicationAlreadyExistsException.h"
#include "sys/relation-context/domain/exception/FriendApplicationAlreadyProcessedException.h"
#include "sys/relation-context/domain/exception/FriendApplicationHandleUnauthorizedException.h"
#include "sys/relation-context/domain/exception/FriendApplicationNotFoundException.h"
#include "sys/relation-context/domain/service/include/FriendApplicationService.h"
#include "utils/ExceptionTestUtils.h"

class FriendApplicationServiceTest : public testing::Test
{
protected:
    void SetUp() override
    {
        sys::common::component::UserCredentialManager::instance().update("100000001", "token");
        validator = std::make_unique<sys::relation::domain::FriendApplicationValidator>(
            &friendApplicationRepository,
            &friendShipRepository);
        service = std::make_unique<sys::relation::domain::FriendApplicationService>(
            &backendClient,
            validator.get(),
            &friendApplicationRepository,
            &friendShipRepository);
    }

    void TearDown() override
    {
        sys::common::component::UserCredentialManager::instance().update("", "");
    }

    static QSharedPointer<sys::relation::domain::FriendApplication> makePendingApplication()
    {
        return sys::relation::domain::FriendApplication::of(
            "100000001", "FA-PENDING", "100000002", "msg_a", "remark_a", QDateTime::currentDateTime());
    }

    static QSharedPointer<sys::relation::domain::FriendApplication> makeAcceptedApplication()
    {
        auto application = makePendingApplication();
        application->accept();
        return application;
    }

    static QSharedPointer<sys::relation::domain::FriendApplication> makeRejectedApplication()
    {
        auto application = makePendingApplication();
        application->reject();
        return application;
    }

    static QSharedPointer<sys::relation::domain::FriendApplication> makePendingApplicationWithTarget(
        const QString& targetUserId)
    {
        return sys::relation::domain::FriendApplication::of(
            "100000001", "FA-PENDING", targetUserId, "msg_a", "remark_a", QDateTime::currentDateTime());
    }

protected:
    testing::StrictMock<tests::common::mock::MockBackendClient> backendClient;
    testing::StrictMock<tests::common::mock::MockFriendApplicationRepository> friendApplicationRepository;
    testing::StrictMock<tests::common::mock::MockFriendShipRepository> friendShipRepository;
    std::unique_ptr<sys::relation::domain::FriendApplicationValidator> validator;
    std::unique_ptr<sys::relation::domain::FriendApplicationService> service;
};

//----------------------------sendFriendApplication----------------------------
// rule:
// 好友申请的验证消息长度限制为[0, 20]，字符类型不限
// 好友申请的对方备注长度限制为[1, 20]，字符类型不限
// 申请用户不能在好友申请未被处理前，再次向同一用户发送好友申请
// 申请用户和目标用户不能已经是好友关系，否则不允许发送好友申请
// 申请用户和目标用户不能是同一个人
// 好友申请发送成功后，应当在本地仓储新增一条待处理的好友申请记录

TEST_F(FriendApplicationServiceTest, 当验证消息长度不合法时_调用sendFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);
    EXPECT_CALL(backendClient, sendFriendApplication(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::InvalidVerificationMessageException>(
        [&]
        {
            service->sendFriendApplication("100000002", "aaaaaaaaaaaaaaaaaaaaa", "ok_remark");
        },
        "验证消息长度不合法");
}

TEST_F(FriendApplicationServiceTest, 当对方备注长度不合法时_调用sendFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);
    EXPECT_CALL(backendClient, sendFriendApplication(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::InvalidRecipientRemarkException>(
        [&]
        {
            service->sendFriendApplication("100000002", "ok_msg", "bbbbbbbbbbbbbbbbbbbbb");
        },
        "对方备注长度不合法");
}

TEST_F(FriendApplicationServiceTest, 当存在发送给目标用户且状态为Pending状态的好友申请时_调用sendFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(makePendingApplication()));
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);
    EXPECT_CALL(backendClient, sendFriendApplication(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::PendingFriendApplicationAlreadyExistsException>(
        [&]
        {
            service->sendFriendApplication("100000002", "ok_msg", "ok_remark");
        },
        "已存在待处理的好友申请");
}

TEST_F(FriendApplicationServiceTest, 当申请用户和目标用户已经是好友时_调用sendFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(friendShipRepository, of(QStringLiteral("100000001"), QStringLiteral("100000002")))
        .Times(1)
        .WillOnce(testing::Return(sys::relation::domain::FriendShip::of("FS-1", "100000001", "100000002")));
    EXPECT_CALL(backendClient, sendFriendApplication(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendShipAlreadyExistsException>(
        [&]
        {
            service->sendFriendApplication("100000002", "ok_msg", "ok_remark");
        },
        "双方已经是好友");
}

TEST_F(FriendApplicationServiceTest, 当目标用户就是申请用户时_调用sendFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendShipRepository, of(testing::_, testing::_)).Times(0);
    EXPECT_CALL(backendClient, sendFriendApplication(testing::_, testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::ApplyFriendToMyselfException>(
        [&]
        {
            service->sendFriendApplication("100000001", "ok_msg", "ok_remark");
        },
        "不能添加自己为好友");
}

TEST_F(FriendApplicationServiceTest, 当输入合法时_调用validateSendFriendApplicationInputs_应该本地仓储新增好友申请)
{
    const QDateTime applyTime = QDateTime::currentDateTime();
    sys::relation::port::SendFriendApplicationResult backendResult;
    backendResult.friendApplicationId = "FA-NEW";
    backendResult.applyTime = applyTime;

    EXPECT_CALL(friendApplicationRepository,
                of(QStringLiteral("100000001"), QStringLiteral("100000002"), sys::relation::domain::ApplicationStatus::
                    Pending))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(friendShipRepository, of(QStringLiteral("100000001"), QStringLiteral("100000002")))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(backendClient,
                sendFriendApplication(QStringLiteral("100000002"), QStringLiteral("ok_msg"), QStringLiteral("ok_remark")
                ))
        .Times(1)
        .WillOnce(testing::Return(backendResult));

    EXPECT_CALL(friendApplicationRepository, save(testing::_))
        .Times(1)
        .WillOnce(testing::Invoke([&](const QSharedPointer<sys::relation::domain::FriendApplication>& application)
                                                              {
                                                                  ASSERT_NE(application, nullptr);
                                                                  EXPECT_EQ(application->friendApplicationId(),
                                                                            "FA-NEW");
                                                                  EXPECT_EQ(application->applicantUserId(),
                                                                            "100000001");
                                                                  EXPECT_EQ(application->targetUserId(),
                                                                            "100000002");
                                                                  EXPECT_EQ(application->verificationMessageValue(),
                                                                            "ok_msg");
                                                                  EXPECT_EQ(application->recipientRemarkValue(),
                                                                            "ok_remark");
                                                                  EXPECT_EQ(application->applicationStatus(),
                                                                            sys::relation::domain::ApplicationStatus::
                                                                            Pending);
                                                              }));

    const QString friendApplicationId = service->sendFriendApplication("100000002", "ok_msg", "ok_remark");
    EXPECT_EQ(friendApplicationId, "FA-NEW");
}

//----------------------------acceptFriendApplication----------------------------
// rule:
// 已经处理过的好友申请不能再被处理
// 只有好友申请的目标用户是自己时，才允许处理好友申请
TEST_F(FriendApplicationServiceTest, 当好友申请状态为Accepted时_调用acceptFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(QStringLiteral("FA-001")))
        .Times(1)
        .WillOnce(testing::Return(makeAcceptedApplication()));
    EXPECT_CALL(backendClient, acceptFriendApplication(testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendApplicationAlreadyProcessedException>(
        [&]
        {
            service->acceptFriendApplication("FA-001", "给好友的备注");
        },
        "好友申请已被处理");
}

TEST_F(FriendApplicationServiceTest, 当好友申请状态为Rejected时_调用acceptFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(QStringLiteral("FA-001")))
        .Times(1)
        .WillOnce(testing::Return(makeRejectedApplication()));
    EXPECT_CALL(backendClient, acceptFriendApplication(testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendApplicationAlreadyProcessedException>(
        [&]
        {
            service->acceptFriendApplication("FA-001", "给好友的备注");
        },
        "好友申请已被处理");
}

TEST_F(FriendApplicationServiceTest, 当处理好友申请的用户不是目标用户时_调用acceptFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(QStringLiteral("FA-001")))
        .Times(1)
        .WillOnce(testing::Return(makePendingApplicationWithTarget("100000002")));
    EXPECT_CALL(backendClient, acceptFriendApplication(testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendApplicationHandleUnauthorizedException>(
        [&]
        {
            service->acceptFriendApplication("FA-001", "备注");
        },
        "无权处理该好友申请");
}

TEST_F(FriendApplicationServiceTest, 当输入的好友申请id不对应任何好友申请时_调用acceptFriendApplication_应该抛出异常)
{
    EXPECT_CALL(friendApplicationRepository, of(QStringLiteral("FA-404")))
        .Times(1)
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(backendClient, acceptFriendApplication(testing::_, testing::_)).Times(0);
    EXPECT_CALL(friendApplicationRepository, save(testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::relation::domain::FriendApplicationNotFoundException>(
        [&]
        {
            service->acceptFriendApplication("FA-404", "备注");
        },
        "好友申请不存在");
}
