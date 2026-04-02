#include "MessageApplicationServiceTextFixture.h"
//
// Created by 86150 on 2026/4/1.
//

// rule:
// 文本消息长度[1, 1000]
// 好友关系解除后，单聊会话和成员不删除，但不能发送消息了
TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果文本内容不合法_应该给出提示)
{
    QString tooLongText(1001, 'a'); // 1001个字符的文本
    auto response = applicationService->sendTextMessage("chat_session_id", tooLongText);
    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.errMsg.value_or(""), "消息文本长度不合法");
    EXPECT_EQ(response.messageView, std::nullopt);
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果聊天会话不存在_应该给出提示)
{
    // 假设RelationClient没有检查出问题
    EXPECT_CALL(*mockRelationClient, checkSenderHasPermissionToSendMessage(testing::_))
        .Times(1);

    auto response = applicationService->sendTextMessage("non_existent_chat_session_id", "Hello");
    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.errMsg.value_or(""), "聊天会话不存在");
    EXPECT_EQ(response.messageView, std::nullopt);
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果往单聊会话发送消息但已经不是好友了_应该给出提示)
{
    // 构造数据
    const QString peerUserId = "222222222";
    QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();
    auto [friendShipId, privateChatSessionId] = fakeChatApiGateway->createFriendShip(currentUserId, peerUserId);
    fakeChatApiGateway->removeFriendShip(friendShipId); // 删除好友关系，但不删除单聊会话和成员关系

    // 设置RelationClient的预期行为，假设RelationClient没有检查出问题
    EXPECT_CALL(*mockRelationClient, checkSenderHasPermissionToSendMessage(testing::_))
        .Times(1);

    // 执行
    auto response = applicationService->sendTextMessage(privateChatSessionId, "Hello");
    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.errMsg.value_or(""), "你们已经不是好友了");
    EXPECT_EQ(response.messageView, std::nullopt);
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果往群聊会话发送消息但已经不是群聊成员了_应该给出提示)
{
    // 暂时不做这个测试，因为目前没有实现这个功能，等实现了再补充这个测试
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果一切正常_应该成功发送消息_本地仓储应该保存新消息)
{
    // 构造数据
    const QString peerUserId = "222222222";
    QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();
    auto [friendShipId, privateChatSessionId] = fakeChatApiGateway->createFriendShip(currentUserId, peerUserId);

    EXPECT_CALL(*mockRelationClient, checkSenderHasPermissionToSendMessage(testing::_))
        .Times(1);

    auto response = applicationService->sendTextMessage(privateChatSessionId, "Hello");
    EXPECT_TRUE(response.success);
    auto messageView = response.messageView.value();

    // 检查返回的消息视图是否正确
    auto expectedMessageView = contract::message::MessageView{
        .msgId = "mocked_message_id",
        .messageType = contract::message::MessageView::MessageType::Text,
        .senderInfo = {
            .userId = currentUserId,
            .nickname = sys::common::component::UserCredentialManager::instance().getNickname(),
            .avatarFileId = sys::common::component::UserCredentialManager::instance().getAvatarFileId(),
        },
        .isMe = true,
        .timestamp = fakeChatApiGateway->getRecentSendTextMessageResponse().getData().getSendTime(),
        .sequence = (int)fakeChatApiGateway->getRecentSendTextMessageResponse().getData().getSequence(),
        .content = "Hello"
        // timestamp和isMe的值可能需要放宽检查，因为它们在测试中可能会有一些不确定性
    };
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.errMsg, std::nullopt);
    EXPECT_EQ(response.messageView, expectedMessageView);

    // 检查本地仓储是否保存了新消息
    QString msgId = fakeChatApiGateway->getRecentSendTextMessageResponse().getData().getMessageId();
    auto message = messageRepositoryAdapter->of(msgId);
    EXPECT_NE(message, nullptr);
    EXPECT_EQ(message->messageIdValue(), msgId);
    EXPECT_EQ(message->chatSessionIdValue(), privateChatSessionId);
    EXPECT_EQ(message->sendTimeValue(), fakeChatApiGateway->getRecentSendTextMessageResponse().getData().getSendTime());
    EXPECT_EQ(message->senderUserIdValue(), currentUserId);
    EXPECT_EQ(*(message->contentValue()), *sys::message::domain::ContentFactory::createTextContent("Hello"));
}





