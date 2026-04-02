//
// Created by 86150 on 2026/4/2.
//
//
#include "MessageApplicationServiceTextFixture.h"


// ------------------- 获取最近的的N条消息 -------------------
TEST_F(MessageApplicationServiceTestFixture, 获取最近的N条消息时_如果聊天会话还没有消息_应该返回空列表)
{
    auto messsages = applicationService->getRecentMessages("non_existent_chat_session_id", 2);
    EXPECT_TRUE(messsages.isEmpty());
}

// 后期支持多种消息后再扩展该用例
TEST_F(MessageApplicationServiceTestFixture, 获取最近的N条消息时_如果聊天会话只有一条文本消息_应该正确返回消息)
{
    // 构造数据
    QString messageId = "message_id_1";
    QString chatSessionId = "chat_session_id_1";
    int seqInChatSession = 0;
    QString senderUserId = "123456789";
    QDateTime sendTime = QDateTime::currentDateTime();
    QString text = "这是第一条消息";
    auto message = sys::message::domain::Message::ofTextMessage(messageId, chatSessionId, seqInChatSession, sendTime,
                                                                senderUserId, text);
    messageRepositoryAdapter->save(message);
    EXPECT_CALL(*mockUserClient, getUsers(testing::_))
    .Times(1)
    .WillOnce(testing::Return(QHash<QString, sys::common::port::UserInfo>{
                                                          {
                                                              senderUserId,
                                                              sys::common::port::UserInfo{
                                                                  senderUserId, "nickname", "avatar-a"
                                                              }
                                                          }
                                                      }));

    // 执行
    auto messages = applicationService->getRecentMessages(chatSessionId, 2);

    // 验证
    ASSERT_EQ(messages.size(), 1);
    auto messageView = messages.first();
    auto expectedMessageView = contract::message::MessageView{
        .msgId = messageId,
        .messageType = contract::message::MessageView::MessageType::Text,
        .senderInfo = {
            .userId = senderUserId,
            .nickname = "nickname",
            .avatarFileId = "avatar-a",
        },
        .isMe = false,
        .timestamp = sendTime,
        .sequence = seqInChatSession,
        .content = "这是第一条消息"
    };
}

TEST_F(MessageApplicationServiceTestFixture, 获取最近的N条消息时_如果聊天会话中有我的消息和别人的消息_应该能正确区分哪些消息是我发的哪些消息是别人发的_返回的多条消息的seq字段是连续递增的)
{
    QString userId1 = "100000002";
    EXPECT_CALL(*mockUserClient, getUsers(QSet{userId1}))
    .Times(1)
    .WillOnce(testing::Return(QHash<QString, sys::common::port::UserInfo>{
                                                             {
                                                                 userId1,
                                                                 sys::common::port::UserInfo{
                                                                     userId1, "Friend-A", "avatar-a"
                                                                 }
                                                             }
                                                         }));
    // 构造数据
    QString text = "这是第一条消息";
    QString chatSessionId = "chat_session_id_1";

    auto message1 = sys::message::domain::Message::ofTextMessage("message_id_1", chatSessionId, 0,
                                                                 QDateTime::currentDateTime(),
                                                                 userId1, "hello");
    auto message2 = sys::message::domain::Message::ofTextMessage("message_id_2", chatSessionId, 1,
                                                                 QDateTime::currentDateTime(),
                                                                 currentUserId, "hello");
    messageRepositoryAdapter->saveAll({message1, message2});

    // 执行
    auto messages = applicationService->getRecentMessages(chatSessionId, 2);

    EXPECT_EQ(messages.size(), 2);
    auto messageView1 = messages[0];
    auto messageView2 = messages[1];
    EXPECT_EQ(messageView1.isMe, false);
    EXPECT_EQ(messageView2.isMe, true);
    EXPECT_EQ(messageView1.sequence, 0);
    EXPECT_EQ(messageView2.sequence, 1);
}

TEST_F(MessageApplicationServiceTestFixture, 获取在某条消息之前的N条消息时_如果消息数量m小于N_应该返回m条消息)
{
    QString userId1 = "100000002";
    EXPECT_CALL(*mockUserClient, getUsers(QSet{userId1}))
    .Times(1)
    .WillOnce(testing::Return(QHash<QString, sys::common::port::UserInfo>{
                                                             {
                                                                 userId1,
                                                                 sys::common::port::UserInfo{
                                                                     userId1, "Friend-A", "avatar-a"
                                                                 }
                                                             }
                                                         }));
    // 构造数据
    QString text = "这是第一条消息";
    QString chatSessionId = "chat_session_id_1";

    QString messageId = "message_id_1";
    auto message = sys::message::domain::Message::ofTextMessage(messageId, chatSessionId, 0,
                                                                QDateTime::currentDateTime(),
                                                                userId1, "hello");
    messageRepositoryAdapter->save(message);

    // 执行
    auto messages = applicationService->getRecentMessages(chatSessionId, 2);

    EXPECT_EQ(messages.size(), 1);
    auto messageView = messages[0];
    EXPECT_EQ(messageView.msgId, messageId);
}



