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
            return response;
        }

    public:
        // 对外暴露的设置数据的函数
        QString createFriendShip(int userId1, int userId2) // 返回friendShipId
        {
            // 建立好友关系
            // 创建单聊会话
            // 向单聊会话中添加成员
        }

        void removeFriendShip(const QString& friendShipId)
        {
            // 只删除friendShipId，不删除单聊会话和成员关系
        }

    private:
        // 管理单聊会话，单聊会话成员，好友关系
        QHash<QString, QPair<int, int>> privateChatSessionIdToUserIds; // chatSessionId -> (userId1, userId2)
        QHash<QString, QString> chatSessionIdToFriendShipId; // chatSessionId -> friendShipId
        QSet<int> friendShipIds; // 已存在的friendShipId
    };
}
