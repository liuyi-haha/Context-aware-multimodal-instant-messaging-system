//
// Created by 86150 on 2026/4/2.
//
#include "MessageApplicationServiceTextFixture.h"
#include <QSet>

#include "sys/relation-context/port/client/include/MessageClient.h"
TEST_F(MessageApplicationServiceTestFixture, 批量获取聊天会话最后一条消息信息时_如果聊天会话id列表为空_应该返回空哈希表)
{
    auto infos = applicationService->getChatSessionLastMessageInfos({});
    EXPECT_TRUE(infos.isEmpty());
}

TEST_F(MessageApplicationServiceTestFixture, 批量获取聊天会话最后一条消息信息时_如果聊天会话只有一条文本消息_应该返回最后一条文本消息信息)
{
    const QString chatSessionId = "chat_session_1";
    const auto sendTime = QDateTime::fromString("2026-04-02T13:36:00.000", Qt::ISODateWithMs);
    auto message = sys::message::domain::Message::ofTextMessage(
        "message_1", chatSessionId, 7, sendTime, currentUserId, "last text");
    messageRepositoryAdapter->save(message);

    auto infos = applicationService->getChatSessionLastMessageInfos(QSet<QString>{chatSessionId});
    EXPECT_EQ(infos.size(), 1);
    auto info = infos.value(chatSessionId);
    EXPECT_EQ(info.type, sys::message::application::MessageApplicationService::MessageType::Text);
    EXPECT_EQ(info.text, QString("last text"));
    EXPECT_EQ(info.mediaFileId, std::nullopt);
    EXPECT_EQ(info.sendTime, sendTime);
}

TEST_F(MessageApplicationServiceTestFixture, 批量获取聊天会话最后一条消息信息时_如果有四种不同类型的消息_应该正确返回每种类型的最后一条消息信息)
{
    // 暂时只验证文本消息，后续支持多种消息类型后再完善这个测试
}

TEST_F(MessageApplicationServiceTestFixture, 批量获取聊天会话最后一条消息信息时_如果某条聊天会话没有消息_应该正确返回LastMessageInfo)
{
    auto infos = applicationService->getChatSessionLastMessageInfos(QSet<QString>{QString("new_chat_session_id")});
    EXPECT_EQ(infos.size(), 1);
    auto info = infos.value("new_chat_session_id");
    EXPECT_EQ(info.type, sys::message::application::MessageApplicationService::MessageType::NoMessage);
    EXPECT_EQ(info.text, std::nullopt);
    EXPECT_EQ(info.mediaFileId, std::nullopt);
    EXPECT_EQ(info.sendTime, QDateTime());
}
