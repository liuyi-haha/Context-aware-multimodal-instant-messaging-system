//
// Created by 86150 on 2026/4/1.
//
#include <QString>
#include <gtest/gtest.h>
#include <QDateTime>

#include "sys/message-context/domain/exception/InvalidTextException.h"
#include "sys/message-context/domain/object/include/Message.h"
#include "utils/ExceptionTestUtils.h"


class MessageTest : public ::testing::Test
{
};

// rule:
// 消息的文本内容不得超过1000字符，不得为空字符串
// 先测Message的值对象
// --------------------- TextContent ---------------------
TEST_F(MessageTest, 当文本文本内容不合法时_调用checkText_应该返回false)
{
    QString emptyText = "";
    QString tooLongText(1001, 'a');
    EXPECT_FALSE(sys::message::domain::TextContent::checkText(emptyText));
    EXPECT_FALSE(sys::message::domain::TextContent::checkText(tooLongText));
}

TEST_F(MessageTest, 当文本文本内容不合法时_调用TextContent的构造方法_应该抛出异常)
{
    QString emptyText = "";
    QString tooLongText(1001, 'a');

    tests::utils::expectThrowWithMessage<sys::message::domain::InvalidTextException>(
        [&]
        {
            auto content = sys::message::domain::TextContent::of(emptyText);
        },
        "消息文本长度不合法");

    tests::utils::expectThrowWithMessage<sys::message::domain::InvalidTextException>(
        [&]
        {
            auto content = sys::message::domain::TextContent::of(tooLongText);
        },
        "消息文本长度不合法");
}

TEST_F(MessageTest, 当文本文本内容不合法时_调用TextContent的构造方法_应该成功创建文本内容对象)
{
    QString normalText(1000, 'a');
    auto content = sys::message::domain::TextContent::of(normalText);
    EXPECT_NE(content, nullptr);
}

TEST_F(MessageTest, 调用getContentType_应该返回Text类型)
{
    EXPECT_EQ(sys::message::domain::TextContent::of("hello")->getContentType(),
              sys::message::domain::ContentType::Text);
}


// 再测值对象的工厂方法
//---------------------- ContentFactory ---------------------
TEST_F(MessageTest, 当文本文本内容合法时_调用ContentFactory的createTextContent方法_应该成功创建文本内容对象)
{
    QString normalText(1000, 'a');
    auto content = sys::message::domain::ContentFactory::createTextContent(normalText);
    EXPECT_NE(content, nullptr);
}

// 最后再测Message聚合根和它的工厂方法
TEST_F(MessageTest, 当消息文本内容合法时_调用Message的ofTextMessage方法_应该成功创建文本消息对象)
{
    QDateTime sendTime = QDateTime::currentDateTime();
    auto message = sys::message::domain::Message::ofTextMessage("message_id", "chat_session_id", 10, sendTime,
                                                                "123456789", "hello");
    // 验证值
    EXPECT_EQ(message->messageIdValue(), "message_id");
    EXPECT_EQ(message->chatSessionIdValue(), "chat_session_id");
    EXPECT_EQ(message->seqInChatSessionValue(), 10);
    EXPECT_EQ(message->sendTimeValue(), sendTime);
    EXPECT_EQ(message->senderUserIdValue(), "123456789");
    auto content = message->contentValue();
    EXPECT_EQ(content->getContentType(), sys::message::domain::ContentType::Text);

    auto textContent = dynamic_cast<sys::message::domain::TextContent*>(content.get());
    EXPECT_NE(textContent, nullptr);
    EXPECT_EQ(textContent->textValue(), "hello");
}

TEST_F(MessageTest, 当消息文本内容不合法时_调用Message的ofTextMessage方法_应该抛出异常)
{
    QString tooLongText(1001, 'a');
    tests::utils::expectThrowWithMessage<sys::message::domain::InvalidTextException>(
        [&]
        {
            sys::message::domain::Message::ofTextMessage("message_id", "chat_session_id", 10,
                                                         QDateTime::currentDateTime(), "123456789",
                                                         tooLongText);
        },
        "消息文本长度不合法");
}
