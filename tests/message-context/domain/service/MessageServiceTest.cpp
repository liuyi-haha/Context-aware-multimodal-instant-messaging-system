#include "gmock/gmock-nice-strict.h"
#include "gtest/gtest.h"
#include "message-context/mock/BackendClientMock.h"
#include "message-context/mock/MessageRepositoryMock.h"
#include "message-context/mock/RelationClientMock.h"
#include "sys/message-context/domain/exception/FriendShipNotFoundException.h"
#include "sys/message-context/domain/exception/InvalidTextException.h"
#include "sys/message-context/domain/service/include/MessageService.h"
#include "sys/message-context/domain/service/include/SenderValidator.h"
#include "utils/ExceptionTestUtils.h"
//
// Created by 86150 on 2026/4/1.
//
class MessageServiceTest : public testing::Test
{
protected:
    void SetUp() override
    {
        // 准备Mock端口
        mockBackendClient = std::make_unique<testing::StrictMock<tests::message::mock::BackendClientMock>>();
        mockRelationClient = std::make_unique<testing::StrictMock<tests::message::mock::RelationClientMock>>();
        mockMessageRepository = std::make_unique<testing::StrictMock<tests::message::mock::MessageRepositoryMock>>();

        senderValidator = std::make_unique<sys::message::domain::SenderValidator>(
            mockRelationClient.get()
        );
        messageService = std::make_unique<sys::message::domain::MessageService>(
            mockBackendClient.get(), senderValidator.get(), mockMessageRepository.get()
        );
    }

    void TearDown() override
    {
    }

protected:
    std::unique_ptr<testing::StrictMock<tests::message::mock::BackendClientMock>> mockBackendClient;
    std::unique_ptr<testing::StrictMock<tests::message::mock::RelationClientMock>> mockRelationClient;
    std::unique_ptr<testing::StrictMock<tests::message::mock::MessageRepositoryMock>> mockMessageRepository;
    std::unique_ptr<sys::message::domain::MessageService> messageService;
    std::unique_ptr<sys::message::domain::SenderValidator> senderValidator;;
};

TEST_F(MessageServiceTest, 如果文本长度不合法_调用sendTextMessage时_应该抛出异常)
{
    QString tooLongText(1001, 'a'); // 1001个字符的文本
    QString emptyText = "";

    tests::utils::expectThrowWithMessage<sys::message::domain::InvalidTextException>(
        [&]()
        {
            messageService->sendTextMessage("someChatSessionId", tooLongText);
        },
        "消息文本长度不合法"
    );

    tests::utils::expectThrowWithMessage<sys::message::domain::InvalidTextException>(
        [&]()
        {
            messageService->sendTextMessage("someChatSessionId", emptyText);
        },
        "消息文本长度不合法"
    );
}

TEST_F(MessageServiceTest, 如果往单聊会话发送消息但已经不是好友了_调用sendTextMessage时_应该抛出异常)
{
    EXPECT_CALL(*mockRelationClient, checkSenderHasPermissionToSendMessage(testing::_))
        .WillOnce(testing::Throw(sys::message::domain::FriendShipNotFoundException()));
    EXPECT_CALL(*mockBackendClient, sendTextMessage(testing::_, testing::_)).Times(0);

    tests::utils::expectThrowWithMessage<sys::message::domain::FriendShipNotFoundException>(
        [&]()
        {
            messageService->sendTextMessage("single_chat_1", "hello");
        },
        "你们已经不是好友了"
    );
}

TEST_F(MessageServiceTest, 如果往群聊会话发送消息但已经不是群聊成员了_调用sendTextMessage时_应该抛出异常)
{
    GTEST_SKIP() << "暂时不做";
}

TEST_F(MessageServiceTest, 如果文本合法且有权限发送_调用sendTextMessage时_应该成功发送消息)
{
    {
        testing::InSequence seq;
        EXPECT_CALL(*mockRelationClient, checkSenderHasPermissionToSendMessage(QString("single_chat_1")));
        EXPECT_CALL(*mockBackendClient, sendTextMessage);
        EXPECT_CALL(*mockMessageRepository, save);
    }

    EXPECT_NO_THROW(messageService->sendTextMessage("single_chat_1", "hello"));
}
