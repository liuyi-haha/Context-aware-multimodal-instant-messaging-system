//
// Created by 86150 on 2026/3/23.
//
#pragma once

#include <QObject>

#include "contract/system-provider/message-context-provider/MessageView.h"
#include "contract/system-provider/relation-context-provider/ChatSessionView.h"
#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"

namespace ui::common
{
    enum class ConnectionStatus
    {
        Connected,
        Disconnected,
        Connecting
    };

    class UIEventBus final : public QObject
    {
        Q_OBJECT

    public:
        static UIEventBus* instance();

    signals:
        // 给UI发送的
        void navSelected(int index);
        void friendApplicationSelected(const contract::relation::FriendApplicationView& friendApplicationView);
        void chatSessionSelected(const contract::relation::ChatSessionView& chatSessionView);
        void friendApplicationStatusChanged(const QString& applicationId,
                                            contract::relation::FriendApplicationView::Status status);
        void messageSent(const contract::message::MessageView& messageView, const QString& chatSessionId);


        void goToLoginRequested();
        void goToRegisterRequested();
        void loginSuccess();
        void connectionStatusChanged(ConnectionStatus status);
        void friendApplicationSent(const QString& friendApplicationId); // 主动发送好友申请后发这个事件

        // 给后台的上下文发送的
        // todo @liuyi 让好友申请列表订阅这个事件
        void friendApplicationUpdated(const QString& applicationId); // 新收到好友申请和好友申请被处理都会发送这个事件
        // todo @liuyi 让RightStack订阅这个事件
        void messageReceived(const QString& sessionId, const QString& messageId);

        // UI和后台上下文都会发送的
        // todo @liuyi 让ChatSessionList订阅这个事件
        void chatSessionUpdated(const QString& privateChatSessionId);

    private:
        explicit UIEventBus(QObject* parent = nullptr);
    };
}
