//
// Created by 86150 on 2026/3/25.
//

#include "../include/RelationApplicationService.h"

#include <exception>

#include "notifications.pb.h"
#include "sys/common/exception/InfraExcception.h"
#include "sys/relation-context/application/assembler/include/FriendApplicationViewAssembler.h"
#include "ui/common/UIEVentBus.h"

namespace sys::relation::application
{
    RelationApplicationService::RelationApplicationService(
        domain::FriendApplicationService* friendApplicationService,
        FriendApplicationViewAssembler* friendApplicationViewAssembler,
        domain::PrivateChatSessionService* privateChatSessionService,
        ChatSessionViewAssembler* chatSessionViewAssembler,
        QObject* parent)
        : QObject(parent),
          friendApplicationService(friendApplicationService),
          friendApplicationViewAssembler(friendApplicationViewAssembler),
          privateChatSessionService(privateChatSessionService),
          chatSessionViewAssembler(chatSessionViewAssembler)
    {
    }

    contract::relation::SendFriendApplicationResponse RelationApplicationService::sendFriendApplication(
        const contract::relation::SendFriendApplicationRequest& request)
    {
        contract::relation::SendFriendApplicationResponse response{};
        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            response.friendApplicationId = friendApplicationService->sendFriendApplication(
                request.targetUserId,
                request.verificationMessage,
                request.recipientRemark);
            response.success = true;
            qDebug() << "RelationApplicationService say: 发送好友申请成功, friendApplicationId:" << response.
                friendApplicationId;
        }
        catch (const std::exception& ex)
        {
            qDebug() << "RelationApplicationService say: 发送好友申请失败:" << QString(ex.what());
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }
        return response;
    }

    contract::relation::AcceptFriendApplicationResponse RelationApplicationService::acceptFriendApplication(
        const contract::relation::AcceptFriendApplicationRequest& request)
    {
        contract::relation::AcceptFriendApplicationResponse response{};
        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            auto [isNewFriendShip, privateChatSessionId] = friendApplicationService->acceptFriendApplication(
                request.applicationId, request.recipientRemark);
            response.success = true;
            response.isNewFriendShip = isNewFriendShip;
            response.privateChatSessionId = privateChatSessionId;
            qDebug() << "RelationApplicationService say: 同意好友申请成功, applicationId:" << request.applicationId
                << "isNewFriendShip:" << isNewFriendShip << "privateChatSessionId:" << privateChatSessionId;

            return response;
        }
        catch (const std::exception& ex)
        {
            qDebug() << "RelationApplicationService say: 同意好友申请失败:" << QString(ex.what());
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
        }

        return response;
    }

    contract::relation::RejectFriendApplicationResponse RelationApplicationService::rejectFriendApplication(
        const contract::relation::RejectFriendApplicationRequest& request)
    {
        contract::relation::RejectFriendApplicationResponse response{};
        if (!checkConfigAndSetResponse(response))
        {
            return response;
        }

        try
        {
            friendApplicationService->rejectFriendApplication(request.friendApplicationId);
            response.success = true;
            qDebug() << "RelationApplicationService say: 拒绝好友申请成功, applicationId:" << request.friendApplicationId;
        }
        catch (const std::exception& ex)
        {
            response.success = false;
            response.errMsg = QString::fromStdString(ex.what());
            qDebug() << "RelationApplicationService say: 拒绝好友申请失败:" << QString(ex.what());
        }

        return response;
    }

    QList<contract::relation::FriendApplicationView> RelationApplicationService::getFriendApplications()
    {
        checkConfig();

        try
        {
            const auto applications = friendApplicationService->getFriendApplications();
            qDebug() << "RelationApplicationService say: 获取好友申请列表成功, application count:" << applications.size();
            return friendApplicationViewAssembler->toFriendApplicationViews(applications);
        }
        catch (const std::exception& ex)
        {
            qDebug() << "获取好友申请列表失败:" << QString(ex.what());
            return {};
        }
    }

    std::optional<contract::relation::FriendApplicationView> RelationApplicationService::getFriendApplication(
        const QString& applicationId)
    {
        checkConfig();

        try
        {
            const auto application = friendApplicationService->getFriendApplication(applicationId);
            auto list = friendApplicationViewAssembler->toFriendApplicationViews({application});
            if (list.empty())
            {
                qDebug() << "RelationApplicationService say: 获取好友申请失败: 没有找到该好友申请";
                return std::nullopt;
            }
            qDebug() << "RelationApplicationService say: 获取好友申请成功, applicationId:" << applicationId;
            return list.first();
        }
        catch (const std::exception& ex)
        {
            qDebug() << "RelationApplicationService say: 获取好友申请失败:" << QString(ex.what());
            return std::nullopt;
        }
    }

    QList<contract::relation::ChatSessionView> RelationApplicationService::getChatSessions()
    {
        checkConfig();
        try
        {
            QList<QSharedPointer<domain::PrivateChatSession>> privateChatSessions = privateChatSessionService->
                getPrivateChatSessions();
            qDebug() << "RelationApplicationService say: 获取聊天会话列表成功, chat session count:" << privateChatSessions.size();
            return chatSessionViewAssembler->toChatSessionViewsByLastMessageSendTimeDesc(privateChatSessions);
        }
        catch (const std::exception& ex)
        {
            qDebug() << "RelationApplicationService say: 获取聊天会话列表失败:" << QString(ex.what());
            return {};
        }
    }

    std::optional<contract::relation::ChatSessionView> RelationApplicationService::getChatSession(
        const QString& chatSessionId)
    {
        checkConfig();
        try
        {
            auto session = privateChatSessionService->getPrivateChatSession(chatSessionId);
            if (session == nullptr)
            {
                qDebug() << "RelationApplicationService say: 获取聊天会话失败: 没有找到该聊天会话, chatSessionId:" << chatSessionId;
                return std::nullopt;
            }
            qDebug() << "RelationApplicationService say: 获取聊天会话成功, chatSessionId:" << chatSessionId;

            auto chatSessionLists = chatSessionViewAssembler->toChatSessionViewsByLastMessageSendTimeDesc({session});
            if (chatSessionLists.empty())
            {
                qDebug() << "RelationApplicationService say: 获取聊天会话失败: 转换为ChatSessionView失败, chatSessionId:" <<
                    chatSessionId;
                return std::nullopt;
            }
            return chatSessionLists.first();
        }
        catch (const std::exception& ex)
        {
            qDebug() << "RelationApplicationService say: 获取聊天会话失败:" << QString(ex.what());
            return std::nullopt;
        }
    }

    void RelationApplicationService::checkSenderHasPermissionToSendMessage(const QString& chatSessionId)
    {
        privateChatSessionService->checkCurrentUserHasPermissionToSendMessage(chatSessionId);
    }

    void RelationApplicationService::handleFriendApplicationReceived(const QByteArray& notificationBytes)
    {
        try
        {
            // 先反序列化
            proto::notification::FriendApplicationReceivedNotification notification;
            assert(notification.ParseFromArray(notificationBytes.data(), notificationBytes.size()));
            QString applicationId = QString::fromStdString(notification.application_id());
            // 再调用领域服务的方法
            friendApplicationService->receiveFriendApplication(
                applicationId,
                QString::fromStdString(notification.applicant_user_id()),
                QString::fromStdString(notification.verification_message()),
                QDateTime::fromString(QString::fromStdString(notification.send_time()), Qt::ISODate));
            // 再让通知上下文通知UI更新界面, 这里直接发送信号

            qDebug() << "RelationApplicationService say: 收到好友申请通知, applicationId:" << applicationId <<
                ", 已发射friendApplicationReceived信号";
            emit ui::common::UIEventBus::instance()->friendApplicationUpdated(applicationId);
        }
        catch (const std::exception& ex)
        {
            qDebug() << "处理好友申请通知时发生异常:" << QString::fromStdString(ex.what());
        }
    }

    void RelationApplicationService::handleFriendApplicationHandled(const QByteArray& notificationBytes)
    {
        // 先反序列化
        proto::notification::FriendApplicationHandledNotification notification;
        assert(notification.ParseFromArray(notificationBytes.data(), notificationBytes.size()));
        QString applicationId = QString::fromStdString(notification.application_id());
        // 再调用领域服务的方法
        try
        {
            switch (notification.result_type())
            {
            case proto::notification::HANDLE_RESULT_APPROVED:
                {
                    bool isNewFriendship = notification.is_new_friendship();
                    if (isNewFriendship)
                    {
                        qDebug() << "好友申请被批准了，并且建立了新的好友关系，applicationId:" << applicationId;
                        friendApplicationService->acceptFriendApplicationByPeerWithNewFriendShip(
                            applicationId,
                            QString::fromStdString(notification.friendship_id()),
                            QString::fromStdString(notification.private_chat_session_id()),
                            QString::fromStdString(notification.applicant_participant_id()),
                            QString::fromStdString(notification.target_user_participant_id()));
                    }
                    else
                    {
                        friendApplicationService->acceptFriendApplicationByPeerWithNoFriendShip(applicationId);
                        qDebug() << "好友申请被批准了，但没有建立新的好友关系，applicationId:" << applicationId;
                    }
                    break;
                }
            case proto::notification::HANDLE_RESULT_REJECTED:
                friendApplicationService->rejectFriendApplicationByPeer(
                    QString::fromStdString(notification.application_id()));
                break;
            default:
                qDebug() << "未知的好友申请处理结果类型:" << notification.result_type();
                break;
            }

            qDebug() << "RelationApplicationService say: 收到好友申请被处理的通知, applicationId:" << applicationId <<
                ", 已发射friendApplicationUpdated信号";
            emit ui::common::UIEventBus::instance()->friendApplicationUpdated(applicationId);
            if (notification.is_new_friendship())
            {
                // 聊天会话新建了，要通知UI更新界面
                emit ui::common::UIEventBus::instance()->chatSessionUpdated(
                    QString::fromStdString(notification.private_chat_session_id()));
            }

            // todo @liuyi 后续还要更新好友列表
        }
        catch (const std::exception& ex)
        {
            qDebug() << "处理好友申请被处理的通知时发生异常:" << QString::fromStdString(ex.what());
        }
    }


    RelationApplicationService::CheckSendMessagePermissionResponse RelationApplicationService::
    checkSendMessagePermission()
    {
        //todo @liuyi
        return CheckSendMessagePermissionResponse{};
    }

    bool RelationApplicationService::checkConfigAndSetResponse(contract::BaseResponse& response)
    {
        if (friendApplicationService == nullptr || friendApplicationViewAssembler
            == nullptr || privateChatSessionService == nullptr || chatSessionViewAssembler == nullptr)
        {
            response.success = false;
            response.errMsg = QStringLiteral("RelationApplicationService is not properly configured.");
            return false;
        }

        return true;
    }

    void RelationApplicationService::checkConfig() const
    {
        if (friendApplicationService == nullptr || friendApplicationViewAssembler == nullptr ||
            privateChatSessionService == nullptr || chatSessionViewAssembler == nullptr)
        {
            throw core::InfraException("RelationApplicationService is not properly configured.");
        }
    }
}
