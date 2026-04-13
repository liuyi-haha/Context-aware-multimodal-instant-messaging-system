//
// Created by 86150 on 2026/3/29.
//
#include <gtest/gtest.h>

#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/exception/FriendApplicationAlreadyProcessedException.h"
#include "sys/relation-context/domain/exception/InvalidRecipientRemarkException.h"
#include "sys/relation-context/domain/exception/InvalidVerificationMessageException.h"
#include "sys/relation-context/domain/object/include/FriendApplication.h"

class FriendApplicationTest : public testing::Test
{
protected:
    static QSharedPointer<sys::relation::domain::FriendApplication> makeApplication(
        const QString& friendApplicationId,
        const QString& applicantUserId,
        const QString& targetUserId,
        const qint64 secsSinceEpoch)
    {
        return sys::relation::domain::FriendApplication::of(
            applicantUserId,
            friendApplicationId,
            targetUserId,
            QStringLiteral("验证消息"),
            QStringLiteral("对方备注"),
            QDateTime::fromSecsSinceEpoch(secsSinceEpoch));
    }

    void SetUp() override
    {
        // 在每个测试用例执行前运行，可以在这里进行一些初始化操作
        sys::common::component::UserCredentialManager::instance().update("100000001", "mock_token", "刘奕",
                                                                         "刘奕的头像", "description",
                                                                         "1234567890");
    }
};

// rule:
// 好友申请的验证消息长度限制为[0, 20], 字符类型不限
// 好友申请的对方备注长度限制为[1, 20], 字符类型不限
// 已经被处理的好友申请无法再次被处理
TEST_F(FriendApplicationTest, 当好友申请的验证消息不合法时_调用VerifyMessage的静态check方法应该返回false)
{
    EXPECT_FALSE(sys::relation::domain::VerificationMessage::checkVerificationMessage(
        QStringLiteral("长度超限123456789012345678901")));
}

TEST_F(FriendApplicationTest, 当好友申请的验证消息不合法时_构造FriendApplication对象_应该抛出异常)
{
    EXPECT_THROW(
        sys::relation::domain::FriendApplication::of(
            "100000001",
            "FA001",
            "100000002",
            "长度超过限制的验证消息11111111111111111111111111111111111111",
            QStringLiteral("备注"),
            QDateTime::currentDateTime()),
        sys::relation::domain::InvalidVerificationMessageException);
}

TEST_F(FriendApplicationTest, 当好友申请的对方备注不合法时_调用RecipientRemark的静态check方法_应该返回false)
{
    EXPECT_FALSE(sys::core::RecipientRemark::checkRecipientRemark(
        QString(21, 'a'))); // 长度超过20
}

TEST_F(FriendApplicationTest, 当好友申请的对方备注不合法时_构造FriendApplication对象_应该抛出异常)
{
    EXPECT_THROW(
        sys::relation::domain::FriendApplication::of(
            "100000001",
            "FA002",
            "100000002",
            QStringLiteral("你好_abc"),
            QString(21, 'a'), // 长度超过20
            QDateTime::currentDateTime()),
        sys::relation::domain::InvalidRecipientRemarkException);
}

TEST_F(FriendApplicationTest, 当构造好友申请的参数合法时_构造FriendApplication对象_应该能成功构造对象)
{
    const auto application = sys::relation::domain::FriendApplication::of(
        "100000001",
        "FA003",
        "100000002",
        QStringLiteral("你好 abc_"),
        QStringLiteral("对方备注_ABC"),
        QDateTime::currentDateTime());

    ASSERT_NE(application, nullptr);
    EXPECT_EQ(application->applicantUserId(), "100000001");
    EXPECT_EQ(application->targetUserId(), "100000002");
    EXPECT_EQ(application->verificationMessageValue(), QStringLiteral("你好 abc_"));
    EXPECT_EQ(application->recipientRemarkValue(), QStringLiteral("对方备注_ABC"));
    EXPECT_EQ(application->applicationStatus(), sys::relation::domain::ApplicationStatus::Pending);
}

TEST_F(FriendApplicationTest, 当好友申请的状态不是Pending时_调用accept方法_应该抛出异常)
{
    const auto application = sys::relation::domain::FriendApplication::of(
        "100000001",
        "FA004",
        "100000002",
        QStringLiteral("验证消息"),
        QStringLiteral("对方备注"),
        QDateTime::currentDateTime());

    application->accept();

    EXPECT_THROW(application->accept(), sys::relation::domain::FriendApplicationAlreadyProcessedException);
}

TEST_F(FriendApplicationTest, 当好友申请的状态不是Pending时_调用reject方法_应该抛出异常)
{
    const auto application = sys::relation::domain::FriendApplication::of(
        "100000001",
        "FA005",
        "100000002",
        QStringLiteral("验证消息"),
        QStringLiteral("对方备注"),
        QDateTime::currentDateTime());

    application->reject();

    EXPECT_THROW(application->reject(), sys::relation::domain::FriendApplicationAlreadyProcessedException);
}

//---------------------------- composeAndSortByApplyTimeDesc ----------------------------
// rule:
// 好友申请列表应该按照申请时间降序排列

TEST_F(FriendApplicationTest, 当我发出的申请和发给我的申请同时存在且时间无序时_调用composeAndSortByApplyTimeDesc_应该按申请时间降序返回)
{
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asApplicant{
        makeApplication("FA101", "100000001", "100000002", 100),
        makeApplication("FA102", "100000001", "100000003", 300)
    };
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asTarget{
        makeApplication("FA103", "100000004", "100000001", 200),
        makeApplication("FA104", "100000005", "100000001", 400)
    };

    const auto result = sys::relation::domain::FriendApplication::composeAndSortByApplyTimeDesc(asApplicant, asTarget);

    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(result[0]->friendApplicationId(), "FA104");
    EXPECT_EQ(result[1]->friendApplicationId(), "FA102");
    EXPECT_EQ(result[2]->friendApplicationId(), "FA103");
    EXPECT_EQ(result[3]->friendApplicationId(), "FA101");
}

TEST_F(FriendApplicationTest, 当只有我发出的申请时_调用composeAndSortByApplyTimeDesc_应该按申请时间降序返回)
{
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asApplicant{
        makeApplication("FA201", "100000001", "100000002", 10),
        makeApplication("FA202", "100000001", "100000003", 30),
        makeApplication("FA203", "100000001", "100000004", 20)
    };
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asTarget;

    const auto result = sys::relation::domain::FriendApplication::composeAndSortByApplyTimeDesc(asApplicant, asTarget);

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0]->friendApplicationId(), "FA202");
    EXPECT_EQ(result[1]->friendApplicationId(), "FA203");
    EXPECT_EQ(result[2]->friendApplicationId(), "FA201");
}

TEST_F(FriendApplicationTest, 当只有发给我的申请时_调用composeAndSortByApplyTimeDesc_应该按申请时间降序返回)
{
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asApplicant;
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asTarget{
        makeApplication("FA301", "100000002", "100000001", 11),
        makeApplication("FA302", "100000003", "100000001", 33),
        makeApplication("FA303", "100000004", "100000001", 22)
    };

    const auto result = sys::relation::domain::FriendApplication::composeAndSortByApplyTimeDesc(asApplicant, asTarget);

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0]->friendApplicationId(), "FA302");
    EXPECT_EQ(result[1]->friendApplicationId(), "FA303");
    EXPECT_EQ(result[2]->friendApplicationId(), "FA301");
}

TEST_F(FriendApplicationTest, 当没有任何好友申请时_调用composeAndSortByApplyTimeDesc_应该返回空列表)
{
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asApplicant;
    QList<QSharedPointer<sys::relation::domain::FriendApplication>> asTarget;

    const auto result = sys::relation::domain::FriendApplication::composeAndSortByApplyTimeDesc(asApplicant, asTarget);

    EXPECT_TRUE(result.isEmpty());
}
