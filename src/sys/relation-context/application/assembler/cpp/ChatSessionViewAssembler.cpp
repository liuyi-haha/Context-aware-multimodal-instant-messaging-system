#include "../include/ChatSessionViewAssembler.h"

#include "sys/relation-context/domain/object/include/PrivateChatSession.h"
#include <algorithm>

#include "sys/common/exception/InfraExcception.h"
#include "sys/common/component/UserCredentialManager.h"

namespace
{
    QList<QString> collectSessionIds(
        const QList<QSharedPointer<sys::relation::domain::PrivateChatSession>>& chatSessions)
    {
        QList<QString> sessionIds;
        for (const auto& chatSession : chatSessions)
        {
            if (chatSession)
            {
                sessionIds.append(chatSession->privateChatSessionId());
            }
        }
        return sessionIds;
    }

    QHash<QString, int> collectUnreadCounts(const QList<QString>& sessionIds,
                                            sys::relation::port::NotificationClient* notificationClient)
    {
        QHash<QString, int> unreadCounts;
        for (const auto& sessionId : sessionIds)
        {
            unreadCounts[sessionId] = notificationClient->getUnreadChatSessionItemCount(sessionId);
        }
        return unreadCounts;
    }

    struct PeerContext
    {
        QSet<QString> otherUserIds;
        QHash<QString, QString> sessionIdToOtherUserId;
        QHash<QString, QString> sessionIdToOtherUserRemark;
    };

    PeerContext resolvePeerContext(const QList<QSharedPointer<sys::relation::domain::PrivateChatSession>>& chatSessions,
                                   sys::relation::port::ParticipantRepository* participantRepository,
                                   const QString& currentUserId)
    {
        PeerContext context;

        for (const auto& chatSession : chatSessions)
        {
            if (!chatSession)
            {
                continue;
            }

            const QString sessionId = chatSession->privateChatSessionId();
            const auto participants = participantRepository->ofChatSessionId(sessionId);

            if (participants.isEmpty())
            {
                continue;
            }

            for (const auto& participant : participants)
            {
                if (!participant || participant->userIdValue() == currentUserId)
                {
                    continue;
                }

                context.otherUserIds.insert(participant->userIdValue());
                context.sessionIdToOtherUserId[sessionId] = participant->userIdValue();
                if (participant->hasRemark())
                {
                    context.sessionIdToOtherUserRemark[sessionId] = participant->remarkValue();
                }
                break;
            }
        }

        return context;
    }

    QString buildLastMessageSummary(const sys::relation::port::MessageClient::LastMessageInfo& lastMessageInfo)
    {
        switch (lastMessageInfo.type)
        {
        case sys::relation::port::MessageClient::MessageType::Text:
            if (lastMessageInfo.text.has_value())
            {
                return lastMessageInfo.text.value();
            }
            break;
        case sys::relation::port::MessageClient::MessageType::Image:
            return "[图片]";
        case sys::relation::port::MessageClient::MessageType::Document:
            if (lastMessageInfo.fileName.has_value())
            {
                return QString("[文件] %1").arg(lastMessageInfo.fileName.value());
            }
            return "[文件]";
        case sys::relation::port::MessageClient::MessageType::Speech:
            return "[语音]";
        }

        return {};
    }
}

namespace sys::relation::application
{
    QList<contract::relation::ChatSessionView> ChatSessionViewAssembler::toChatSessionViewsByLastMessageSendTimeDesc(
        const QList<QSharedPointer<domain::PrivateChatSession>>& chatSessions)
    {
        checkConfig();

        QList<contract::relation::ChatSessionView> views = toChatSessionViews(chatSessions);
        // 按照最后一条消息的发送时间降序排序
        std::sort(views.begin(), views.end(),
                  [](const contract::relation::ChatSessionView& a, const contract::relation::ChatSessionView& b)
                  {
                      return a.lastMessageSendTime > b.lastMessageSendTime;
                  });

        return views;
    }

    QList<contract::relation::ChatSessionView> ChatSessionViewAssembler::toChatSessionViews(
        const QList<QSharedPointer<domain::PrivateChatSession>>& chatSessions)
    {
        checkConfig();

        if (chatSessions.isEmpty())
        {
            return {};
        }

        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        const QList<QString> sessionIds = collectSessionIds(chatSessions);
        const auto lastMessageInfos = messageClient->getLastMessageInfos(sessionIds);
        const auto unreadCounts = collectUnreadCounts(sessionIds, notificationClient);
        const auto peerContext = resolvePeerContext(chatSessions, participantRepository, currentUserId);
        const auto userInfos = userClient->getUsers(peerContext.otherUserIds);

        QList<contract::relation::ChatSessionView> views;

        for (const auto& chatSession : chatSessions)
        {
            if (!chatSession)
            {
                continue;
            }

            const QString sessionId = chatSession->privateChatSessionId();
            const QString otherUserId = peerContext.sessionIdToOtherUserId.value(sessionId);
            if (otherUserId.isEmpty())
            {
                continue;
            }

            const auto userInfoIt = userInfos.find(otherUserId);
            if (userInfoIt == userInfos.end())
            {
                continue;
            }

            const QString peerRemark = peerContext.sessionIdToOtherUserRemark.value(sessionId);
            if (peerRemark.isEmpty())
            {
                continue;
            }

            const auto lastMessageInfoIt = lastMessageInfos.find(sessionId);
            const int unreadCount = unreadCounts.value(sessionId, 0);

            QString lastMessageSummary;
            QDateTime lastMessageSendTime;
            if (lastMessageInfoIt != lastMessageInfos.end())
            {
                const auto& lastMessageInfo = lastMessageInfoIt.value();
                lastMessageSendTime = lastMessageInfo.sendTime;
                lastMessageSummary = buildLastMessageSummary(lastMessageInfo);
            }

            contract::relation::ChatSessionView view;
            view.chatSessionId = sessionId;
            view.chatSessionType = contract::relation::ChatSessionType::Private;
            view.name = peerRemark;
            view.avatarFileId = userInfoIt.value().avatarFileId;
            view.avatar = std::nullopt; // 头像数据暂时为空，后续可以异步加载
            view.lastMessageSummary = lastMessageSummary;
            view.lastMessageSendTime = lastMessageSendTime;
            view.unreadCount = unreadCount;

            views.append(view);
        }

        return views;
    }

    void ChatSessionViewAssembler::checkConfig() const
    {
        if (messageClient == nullptr || notificationClient == nullptr || participantRepository == nullptr || userClient
            == nullptr)
        {
            throw core::InfraException("ChatSessionViewAssembler dependencies are not fully configured.");
        }
    }
}
