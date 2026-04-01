#include "MessageApplicationServiceTextFixture.h"
//
// Created by 86150 on 2026/4/1.
//

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果文本内容不合法_应该给出提示)
{
    // 跳过
    GTEST_SKIP() << "暂时不做";
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果聊天会话不存在_应该给出提示)
{
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果往单聊会话发送消息但已经不是好友了_应该给出提示)
{
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果往群聊会话发送消息但已经不是群聊成员了_应该给出提示)
{
}

TEST_F(MessageApplicationServiceTestFixture, 发送文本消息时_如果一切正常_应该成功发送消息_本地仓储应该保存新消息)
{
}





