//
// Created by 86150 on 2026/3/31.
//
#include "relation-context/application/service/RelationApplicationServiceTestFixture.h"

//------------------------------- 获取所有聊天会话 -------------------------------
// rule:
// 聊天会话应该按照最后一条消息的发送时间降序排序
// 聊天会话要显示最后一条消息的摘要, 如果是文本消息，则为文本内容；若为文件消息，则为"[文件] 文件名(包含后缀)"；如果是图片消息，则为"[图片]"；如果是语音消息，则为"[语音]"
// 如果是单聊会话，名称栏应显示给好友的对方备注
// 每个聊天会话都要显示未读消息数量
// 每个聊天会话都要显示最后一条消息的发送时间，如果是当天发送的，就显示时间（如14:20），如果不是当天发送的，就显示日期（如3/31）

namespace tests::common::application::get_chat_sessions
{
    TEST_F(RelationApplicationServiceTestFixture, 获取所有聊天会话时_如果没有聊天会话_应该返回空列表)
    {
        EXPECT_CALL(*mockMessageClient, getLastMessageInfos(testing::_)).Times(0);
        EXPECT_CALL(*mockNotificationClient, getUnreadChatSessionItemCount(testing::_)).Times(0);
        EXPECT_CALL(*mockUserClient, getUsers(testing::_)).Times(0);


        const auto chatSessions = relationApplicationService->getChatSessions();
        EXPECT_TRUE(chatSessions.isEmpty());
    }

    TEST_F(RelationApplicationServiceTestFixture, 获取所有聊天会话时_如果只有一个单聊会话_单聊会话的信息应该完整且正确)
    {
        // 准备本地仓储
        const QString friendShipId = "FS-001";
        const QString privateChatSessionId = "PCS-001";
        const QString peerUserId = "100000002";
        seedPrivateChatSession(friendShipId, privateChatSessionId, peerUserId, "好友备注");

        // 设置Mock期望
        const QDateTime expectedTime = QDateTime::currentDateTime();
        sys::relation::port::MessageClient::LastMessageInfo expectedLastMessageInfo{
            .text = "你好，最近怎么样？",
            .fileName = std::nullopt,
            .type = sys::relation::port::MessageClient::MessageType::Text,
            .sendTime = expectedTime
        };
        auto expectedLastMessageInfos = QHash<QString, sys::relation::port::MessageClient::LastMessageInfo>{
            {privateChatSessionId, expectedLastMessageInfo}
        };

        EXPECT_CALL(*mockMessageClient, getLastMessageInfos(QList<QString>{privateChatSessionId}))
            .Times(1)
            .WillOnce(testing::Return(expectedLastMessageInfos));

        EXPECT_CALL(*mockNotificationClient, getUnreadChatSessionItemCount(privateChatSessionId))
            .Times(1)
            .WillOnce(testing::Return(3));

        EXPECT_CALL(*mockUserClient, getUsers(QSet<QString>{peerUserId}))
        .Times(1)
        .WillOnce(testing::Return(QHash<QString, sys::common::port::UserInfo>{
                                                                             {
                                                                                 peerUserId,
                                                                                 sys::common::port::UserInfo{
                                                                                     peerUserId, "好友昵称", "fileId-123"
                                                                                 }
                                                                             }
                                                                         }));

        // 执行
        QList<contract::relation::ChatSessionView> chatSessions = relationApplicationService->getChatSessions();

        // 验证结果
        ASSERT_EQ(chatSessions.size(), 1);

        const auto& chatSession = chatSessions[0];
        EXPECT_EQ(chatSession.chatSessionId, privateChatSessionId);
        EXPECT_EQ(chatSession.chatSessionType, contract::relation::ChatSessionType::Private);
        EXPECT_EQ(chatSession.name, "好友备注");
        EXPECT_EQ(chatSession.avatarFileId, "fileId-123");
        EXPECT_FALSE(chatSession.avatar.has_value());
        EXPECT_EQ(chatSession.lastMessageSummary, "你好，最近怎么样？");
        EXPECT_EQ(chatSession.lastMessageSendTime, expectedTime);
        EXPECT_EQ(chatSession.unreadCount, 3);
    }

    TEST_F(RelationApplicationServiceTestFixture, 获取所有聊天会话时_如果包含聊天会话的最后一条消息有四种不同类型以及没有消息时_消息摘要栏应该能区别显示)
    {
        // 创建四个聊天会话
        const QString friendShipId1 = "FS-001";
        const QString privateChatSessionId1 = "PCS-001";
        const QString peerUserId1 = "100000002";

        const QString friendShipId2 = "FS-002";
        const QString privateChatSessionId2 = "PCS-002";
        const QString peerUserId2 = "100000003";

        const QString friendShipId3 = "FS-003";
        const QString privateChatSessionId3 = "PCS-003";
        const QString peerUserId3 = "100000004";

        const QString friendShipId4 = "FS-004";
        const QString privateChatSessionId4 = "PCS-004";
        const QString peerUserId4 = "100000005";

        const QString friendShipId5 = "FS-005";
        const QString privateChatSessionId5 = "PCS-005";
        const QString peerUserId5 = "100000006";

        seedPrivateChatSession(friendShipId1, privateChatSessionId1, peerUserId1, "用户1");
        seedPrivateChatSession(friendShipId2, privateChatSessionId2, peerUserId2, "用户2");
        seedPrivateChatSession(friendShipId3, privateChatSessionId3, peerUserId3, "用户3");
        seedPrivateChatSession(friendShipId4, privateChatSessionId4, peerUserId4, "用户4");
        seedPrivateChatSession(friendShipId4, privateChatSessionId5, peerUserId5, "用户5");

        // 设置mock期望 - 四种不同类型的消息
        const QDateTime messageTime = QDateTime::currentDateTime();

        // 文本消息
        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo1;
        lastMessageInfo1.text = "这是一条文本消息";
        lastMessageInfo1.type = sys::relation::port::MessageClient::MessageType::Text;
        lastMessageInfo1.sendTime = messageTime;

        // 图片消息
        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo2;
        lastMessageInfo2.text = std::nullopt;
        lastMessageInfo2.type = sys::relation::port::MessageClient::MessageType::Image;
        lastMessageInfo2.sendTime = messageTime;

        // 文件消息
        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo3;
        lastMessageInfo3.text = std::nullopt;
        lastMessageInfo3.fileName = "文档.pdf";
        lastMessageInfo3.type = sys::relation::port::MessageClient::MessageType::Document;
        lastMessageInfo3.sendTime = messageTime;

        // 语音消息
        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo4;
        lastMessageInfo4.text = std::nullopt;
        lastMessageInfo4.type = sys::relation::port::MessageClient::MessageType::Speech;
        lastMessageInfo4.sendTime = messageTime;

        // 没有消息
        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo5;
        lastMessageInfo5.type = sys::relation::port::MessageClient::MessageType::NoMessage;


        EXPECT_CALL(*mockMessageClient, getLastMessageInfos(testing::_))
        .Times(1)
        .WillOnce(testing::Return(QHash<QString, sys::relation::port::MessageClient::LastMessageInfo>{
                                                                            {privateChatSessionId1, lastMessageInfo1},
                                                                            {privateChatSessionId2, lastMessageInfo2},
                                                                            {privateChatSessionId3, lastMessageInfo3},
                                                                            {privateChatSessionId4, lastMessageInfo4},
                                                                            {privateChatSessionId5, lastMessageInfo5}
                                                                        }));

        // 设置未读消息数量
        EXPECT_CALL(*mockNotificationClient, getUnreadChatSessionItemCount(testing::_))
            .Times(5)
            .WillRepeatedly(testing::Return(0));

        EXPECT_CALL(*mockUserClient, getUsers(QSet<QString>{peerUserId1, peerUserId2, peerUserId3, peerUserId4}))
            .Times(1)
            .WillOnce(testing::Return(QHash<QString, sys::common::port::UserInfo>{
                {peerUserId1, sys::common::port::UserInfo{peerUserId1, "好友昵称", "fileId-123"}},
                {peerUserId2, sys::common::port::UserInfo{peerUserId2, "好友昵称", "fileId-123"}},
                {peerUserId3, sys::common::port::UserInfo{peerUserId3, "好友昵称", "fileId-123"}},
                {peerUserId4, sys::common::port::UserInfo{peerUserId4, "好友昵称", "fileId-123"}},
                {peerUserId5, sys::common::port::UserInfo{peerUserId5, "好友昵称", "fileId-123"}},
            }));

        // 调用获取聊天会话
        const auto chatSessions = relationApplicationService->getChatSessions();

        // 验证结果
        ASSERT_EQ(chatSessions.size(), 5);

        // 验证不同类型的消息摘要
        EXPECT_EQ(chatSessions[0].lastMessageSummary, "这是一条文本消息");
        EXPECT_EQ(chatSessions[1].lastMessageSummary, "[图片]");
        EXPECT_EQ(chatSessions[2].lastMessageSummary, "[文件] 文档.pdf");
        EXPECT_EQ(chatSessions[3].lastMessageSummary, "[语音]");
        EXPECT_EQ(chatSessions[5].lastMessageSummary, "");
    }

    TEST_F(RelationApplicationServiceTestFixture, 获取所有聊天会话时_如果有多个单聊会话_单聊会话应该按照最后一条消息的发送时间降序排序)
    {
        // 创建三个聊天会话
        const QString friendShipId1 = "FS-001";
        const QString privateChatSessionId1 = "PCS-001";
        const QString peerUserId1 = "100000002";

        const QString friendShipId2 = "FS-002";
        const QString privateChatSessionId2 = "PCS-002";
        const QString peerUserId2 = "100000003";

        const QString friendShipId3 = "FS-003";
        const QString privateChatSessionId3 = "PCS-003";
        const QString peerUserId3 = "100000004";

        seedPrivateChatSession(friendShipId1, privateChatSessionId1, peerUserId1);
        seedPrivateChatSession(friendShipId2, privateChatSessionId2, peerUserId2);
        seedPrivateChatSession(friendShipId3, privateChatSessionId3, peerUserId3);

        // 设置mock期望 - 不同时间的消息
        const QDateTime time1 = QDateTime::fromString("2026-03-31 10:00:00", "yyyy-MM-dd HH:mm:ss");
        const QDateTime time2 = QDateTime::fromString("2026-03-31 12:00:00", "yyyy-MM-dd HH:mm:ss");
        const QDateTime time3 = QDateTime::fromString("2026-03-31 14:00:00", "yyyy-MM-dd HH:mm:ss");

        // 时间顺序：time3 > time2 > time1
        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo1;
        lastMessageInfo1.text = "最早的消息";
        lastMessageInfo1.type = sys::relation::port::MessageClient::MessageType::Text;
        lastMessageInfo1.sendTime = time1;

        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo2;
        lastMessageInfo2.text = "中间的消息";
        lastMessageInfo2.type = sys::relation::port::MessageClient::MessageType::Text;
        lastMessageInfo2.sendTime = time2;

        sys::relation::port::MessageClient::LastMessageInfo lastMessageInfo3;
        lastMessageInfo3.text = "最新的消息";
        lastMessageInfo3.type = sys::relation::port::MessageClient::MessageType::Text;
        lastMessageInfo3.sendTime = time3;

        EXPECT_CALL(*mockMessageClient, getLastMessageInfos(testing::_))
                            .Times(1)
                            .WillOnce(testing::Return(
                                                                            QHash<
                                                                                QString,
                                                                                sys::relation::port::MessageClient::LastMessageInfo>
                                                                            {
                                                                                {
                                                                                    privateChatSessionId1,
                                                                                    lastMessageInfo1
                                                                                },
                                                                                {
                                                                                    privateChatSessionId2,
                                                                                    lastMessageInfo2
                                                                                },
                                                                                {
                                                                                    privateChatSessionId3,
                                                                                    lastMessageInfo3
                                                                                }
                                                                            }));

        // 设置未读消息数量
        EXPECT_CALL(*mockNotificationClient, getUnreadChatSessionItemCount(testing::_))
            .Times(3)
            .WillRepeatedly(testing::Return(0));

        EXPECT_CALL(*mockUserClient, getUsers(QSet<QString>{peerUserId1, peerUserId2, peerUserId3}))
            .Times(1)
            .WillOnce(testing::Return(QHash<QString, sys::common::port::UserInfo>{
                {peerUserId1, sys::common::port::UserInfo{peerUserId1, "好友昵称", "fileId-123"}},
                {peerUserId2, sys::common::port::UserInfo{peerUserId2, "好友昵称", "fileId-123"}},
                {peerUserId3, sys::common::port::UserInfo{peerUserId3, "好友昵称", "fileId-123"}},
            }));

        // 调用获取聊天会话
        const auto chatSessions = relationApplicationService->getChatSessions();

        // 验证结果
        ASSERT_EQ(chatSessions.size(), 3);

        // 验证排序：应该按照最后一条消息的发送时间降序排序
        // 最新的消息（time3）应该在第一位
        EXPECT_EQ(chatSessions[0].lastMessageSendTime, time3);
        EXPECT_EQ(chatSessions[0].lastMessageSummary, "最新的消息");

        // 中间的消息（time2）应该在第二位
        EXPECT_EQ(chatSessions[1].lastMessageSendTime, time2);
        EXPECT_EQ(chatSessions[1].lastMessageSummary, "中间的消息");

        // 最早的消息（time1）应该在第三位
        EXPECT_EQ(chatSessions[2].lastMessageSendTime, time1);
        EXPECT_EQ(chatSessions[2].lastMessageSummary, "最早的消息");
    }
}
