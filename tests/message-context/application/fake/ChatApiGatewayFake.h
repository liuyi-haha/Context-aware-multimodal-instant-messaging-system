//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include "sys/message-context/adapter/client/include/BackendClientAdapter.h"

#include <QDateTime>
#include <QHash>
#include <QSet>
#include <QString>

#include "sys/common/component/UserCredentialManager.h"

namespace tests::message::fake
{
    class ChatApiGatewayFake : public sys::message::adapter::IChatApiGateway
    {
    public:
        OpenAPIChat::OAIChatSendTextMessage_200_response sendTextMessage(
            const OpenAPIChat::OAIChatSendTextMessage_request& request,
            const QString& chatSessionId) override
        {
            const QString currentUserId = sys::common::component::UserCredentialManager::instance().getCurrentUserId();
            if (!privateChatSessionIdToUserIds.contains(chatSessionId))
            {
                recentSendTextMessageResponse = buildError("SESSION_NOT_FOUND", "聊天会话不存在");
                return recentSendTextMessageResponse;
            }

            const auto members = privateChatSessionIdToUserIds.value(chatSessionId);
            if (members.first != currentUserId && members.second != currentUserId)
            {
                recentSendTextMessageResponse = buildError("NOT_PARTICIPANT", "你不在该聊天会话中");
                return recentSendTextMessageResponse;
            }

            const QString friendShipId = chatSessionIdToFriendShipId.value(chatSessionId);
            if (!friendShipIds.contains(friendShipId))
            {
                recentSendTextMessageResponse = buildError("FRIENDSHIP_REQUIRED", "你们已经不是好友了");
                return recentSendTextMessageResponse;
            }

            const QString content = request.getContent();
            if (content.isEmpty())
            {
                recentSendTextMessageResponse = buildError("CONTENT_EMPTY", "消息文本长度不合法");
                return recentSendTextMessageResponse;
            }
            if (content.size() > 1000)
            {
                recentSendTextMessageResponse = buildError("CONTENT_TOO_LONG", "消息文本长度不合法");
                return recentSendTextMessageResponse;
            }

            OpenAPIChat::OAIChatSendTextMessage_200_response_data data;
            data.setMessageId(QStringLiteral("mocked_message_id"));
            data.setSequence(nextSequence++);
            data.setSendTime(QDateTime::currentDateTime());

            OpenAPIChat::OAIChatSendTextMessage_200_response response;
            response.setSuccess(true);
            response.setData(data);
            response.setErrCode(QString());
            response.setErrMsg(QString());

            recentSendTextMessageResponse = response;
            return recentSendTextMessageResponse;
        }

    public:
        QPair<QString, QString> createFriendShip(const QString& userId1, const QString& userId2)
        {
            const QString friendShipId = QStringLiteral("friend_ship_") + QString::number(nextFriendShipId++);
            const QString privateChatSessionId = QStringLiteral("private_chat_session_") + QString::number(
                nextPrivateChatSessionId++);

            friendShipIds.insert(friendShipId);
            privateChatSessionIdToUserIds.insert(privateChatSessionId, {userId1, userId2});
            chatSessionIdToFriendShipId.insert(privateChatSessionId, friendShipId);
            return {friendShipId, privateChatSessionId};
        }

        void removeFriendShip(const QString& friendShipId)
        {
            friendShipIds.remove(friendShipId);
        }

        [[nodiscard]] OpenAPIChat::OAIChatSendTextMessage_200_response getRecentSendTextMessageResponse() const
        {
            return recentSendTextMessageResponse;
        }

    private:
        static OpenAPIChat::OAIChatSendTextMessage_200_response buildError(
            const QString& errCode, const QString& errMsg)
        {
            OpenAPIChat::OAIChatSendTextMessage_200_response response;
            response.setSuccess(false);
            response.setErrCode(errCode);
            response.setErrMsg(errMsg);
            return response;
        }

    private:
        QHash<QString, QPair<QString, QString>> privateChatSessionIdToUserIds; // chatSessionId -> (userId1, userId2)
        QHash<QString, QString> chatSessionIdToFriendShipId; // chatSessionId -> friendShipId
        QSet<QString> friendShipIds;
        OpenAPIChat::OAIChatSendTextMessage_200_response recentSendTextMessageResponse;
        qint64 nextSequence = 1;
        int nextFriendShipId = 1;
        int nextPrivateChatSessionId = 1;
    };
}
