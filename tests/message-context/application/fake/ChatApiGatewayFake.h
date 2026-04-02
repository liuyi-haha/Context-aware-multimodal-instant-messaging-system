//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/message-context/adapter/client/include/BackendClientAdapter.h"
#include <QString>

namespace tests::message::fake
{
    class ChatApiGatewayFake : public sys::message::adapter::IChatApiGateway
    {
    public:
        OpenAPIChat::OAIChatSendTextMessage_200_response sendTextMessage(
            const OpenAPIChat::OAIChatSendTextMessage_request& request) override
        {
            OpenAPIChat::OAIChatSendTextMessage_200_response response;
            recentSendTextMessageResponse = response; // 记录最近一次调用的response，方便测试验证
            return response;
        }

    public:
        // 对外暴露的设置数据的函数
        QPair<QString, QString> createFriendShip(const QString& userId1, const QString& userId2)
        // 返回friendShipId和privateChatSessionId
        {
            // 建立好友关系
            // 创建单聊会话
            // 向单聊会话中添加成员
        }

        void removeFriendShip(const QString& friendShipId)
        {
            // 只删除friendShipId，不删除单聊会话和成员关系
        }

        OpenAPIChat::OAIChatSendTextMessage_200_response getRecentSendTextMessageResponse() const
        {
            return recentSendTextMessageResponse;
        }

    private:
        // 管理单聊会话，单聊会话成员，好友关系
        QHash<QString, QPair<int, int>> privateChatSessionIdToUserIds; // chatSessionId -> (userId1, userId2)
        QHash<QString, QString> chatSessionIdToFriendShipId; // chatSessionId -> friendShipId
        QSet<int> friendShipIds; // 已存在的friendShipId
        OpenAPIChat::OAIChatSendTextMessage_200_response recentSendTextMessageResponse;
    };
}
