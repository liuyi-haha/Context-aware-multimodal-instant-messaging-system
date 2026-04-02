//
// Created by 86150 on 2026/3/29.
//
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "relation-context/mock/MockPrivateChatSessionRepository.h"
#include "relation-context/mock/MockParticipantRepository.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/service/include/PrivateChatSessionService.h"

class PrivateChatSessionServiceTest : public testing::Test
{
protected:
    void SetUp() override
    {
        // 设置当前用户ID
        sys::common::component::UserCredentialManager::instance().update("100000001", "token");
    }

    void TearDown() override
    {
        // 清理用户凭证
        sys::common::component::UserCredentialManager::instance().update("", "");
    }

protected:
    testing::StrictMock<tests::common::mock::MockPrivateChatSessionRepository> mockPrivateChatSessionRepository;
    testing::StrictMock<tests::common::mock::MockParticipantRepository> mockParticipantRepository;
};

//----------------------------- getPrivateChatSessions -----------------------------
TEST_F(PrivateChatSessionServiceTest, 当当前用户没有任何单聊会话时_调用getPrivateChatSessions_应该返回一个空列表)
{
    sys::relation::domain::PrivateChatSessionService service(
        &mockPrivateChatSessionRepository,
        &mockParticipantRepository);

    // 模拟当前用户没有参与者记录
    EXPECT_CALL(mockParticipantRepository,
                ofAll(QStringLiteral("100000001"), sys::relation::domain::ParticipantRole::PrivateMember))
        .Times(1)
        .WillOnce(testing::Return(QList<QSharedPointer<sys::relation::domain::Participant>>{}));

    // 由于participants为空，privateChatSessionRepository的ofAll方法不会被调用
    EXPECT_CALL(mockPrivateChatSessionRepository, ofAll(testing::_)).Times(0);

    const auto sessions = service.getPrivateChatSessions();

    EXPECT_TRUE(sessions.isEmpty());
}

TEST_F(PrivateChatSessionServiceTest, 当当前用户有多个单聊会话时_调用getPrivateChatSessions_应该返回包含所有单聊会话的列表)
{
    sys::relation::domain::PrivateChatSessionService service(
        &mockPrivateChatSessionRepository,
        &mockParticipantRepository);

    // 创建模拟会话
    QList<QSharedPointer<sys::relation::domain::PrivateChatSession>> expectedSessions{
        sys::relation::domain::PrivateChatSession::of("SESSION-001", "FS-001"),
        sys::relation::domain::PrivateChatSession::of("SESSION-002", "FS-002"),
        sys::relation::domain::PrivateChatSession::of("SESSION-003", "FS-003")
    };

    // 创建模拟参与者
    QList<QSharedPointer<sys::relation::domain::Participant>> participants{
        sys::relation::domain::Participant::of("P001", "SESSION-001", "100000001",
                                               sys::relation::domain::ParticipantRole::PrivateMember),
        sys::relation::domain::Participant::of("P001", "SESSION-002", "100000001",
                                               sys::relation::domain::ParticipantRole::PrivateMember),
        sys::relation::domain::Participant::of("P001", "SESSION-003", "100000001",
                                               sys::relation::domain::ParticipantRole::PrivateMember),
    };

    // 设置期望
    EXPECT_CALL(mockParticipantRepository,
                ofAll(QStringLiteral("100000001"), sys::relation::domain::ParticipantRole::PrivateMember))
        .Times(1)
        .WillOnce(testing::Return(participants));

    QSet<QString> expectedSessionIds{"SESSION-001", "SESSION-002", "SESSION-003"};
    EXPECT_CALL(mockPrivateChatSessionRepository, ofAll(expectedSessionIds))
        .Times(1)
        .WillOnce(testing::Return(expectedSessions));

    const auto sessions = service.getPrivateChatSessions();

    // 断言结果
    ASSERT_EQ(sessions.size(), 3);
    // 检查每个会话的字段
    for (int i = 0; i < sessions.size(); ++i)
    {
        const auto& session = sessions[i];
        const auto& expectedSession = expectedSessions[i];

        ASSERT_NE(session, nullptr);
        ASSERT_NE(expectedSession, nullptr);

        EXPECT_EQ(session->privateChatSessionId(), expectedSession->privateChatSessionId());
        EXPECT_EQ(session->friendShipId(), expectedSession->friendShipId());
        EXPECT_EQ(session->getType(), sys::relation::domain::ChatSessionType::PrivateChatSessionType);
    }
}
