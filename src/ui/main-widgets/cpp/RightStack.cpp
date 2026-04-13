#include "ui/main-widgets/include/RightStack.h"

#include <QWidget>

#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"
#include "ui/common-widgets/SoundPlayer.h"
#include "ui/common/UIEVentBus.h"
#include "ui/message-widgets/include/MessagePanel.h"
#include "ui/relation-widgets/include/FriendApplicationDetail.h"

namespace ui::main_widgets
{
    RightStack::RightStack(QWidget* parent)
        : QStackedWidget(parent)
    {
        setStyleSheet("background-color: rgb(237, 237, 237);");

        auto* chatPlaceholder = new QWidget();
        chatPlaceholder->setStyleSheet("background-color: white;");

        m_friendApplicationDetail = new ui::relation_widgets::FriendApplicationDetail(this);

        auto* emptyPlaceholder = new QWidget();
        emptyPlaceholder->setStyleSheet("background-color: white;");

        addWidget(chatPlaceholder);
        addWidget(m_friendApplicationDetail);
        addWidget(emptyPlaceholder);
        setCurrentIndex(EmptyPage);

        setupConnections();
    }

    void RightStack::showFriendApplicationDetail(const contract::relation::FriendApplicationView& friendApplicationView)
    {
        if (m_friendApplicationDetail == nullptr)
        {
            return;
        }

        m_friendApplicationDetail->setFriendApplicationView(friendApplicationView);
        setCurrentIndex(FriendApplicationDetailPage);
    }

    void RightStack::setupConnections()
    {
        connect(ui::common::UIEventBus::instance(), &ui::common::UIEventBus::navSelected, this, [this](const int)
        {
            setCurrentIndex(EmptyPage);
        });

        connect(ui::common::UIEventBus::instance(),
                &ui::common::UIEventBus::friendApplicationSelected,
                this,
                [this](const contract::relation::FriendApplicationView& friendApplicationView)
                {
                    showFriendApplicationDetail(friendApplicationView);
                });
        connect(ui::common::UIEventBus::instance(),
                &ui::common::UIEventBus::chatSessionSelected,
                this,
                [this](const contract::relation::ChatSessionView& chatSessionView)
                {
                    // 查找是MessagePanel是否存在，如果不存在则创建
                    if (!chatSessionIdToPageIndex.contains(chatSessionView.chatSessionId))
                    {
                        auto messagePanel = new message_widgets::MessagePanel(
                            chatSessionView.chatSessionId, chatSessionView.name, this);
                        int index = addWidget(messagePanel);
                        chatSessionIdToPageIndex.insert(chatSessionView.chatSessionId, index);
                    }
                    setCurrentIndex(chatSessionIdToPageIndex.value(chatSessionView.chatSessionId, EmptyPage));
                });


        // 连接消息发送信号
        connect(common::UIEventBus::instance(), &common::UIEventBus::messageSent, this,
                [this](const contract::message::MessageView& messageView, const QString& chatSessionId)
                {
                    // 找到MessagePannel
                    auto messagePanel = getMessagePanel(chatSessionId);
                    if (messagePanel == nullptr)
                    {
                        qDebug() << "RightStack::messageSent: 未找到对应的MessagePanel，chatSessionId=" << chatSessionId;
                        return;
                    }
                    auto messageListWidget = findMessageListWidgetInMessagePanel(messagePanel);
                    messageListWidget->addMessages({messageView});
                });

        // 连接消息接收信号
        connect(common::UIEventBus::instance(), &common::UIEventBus::messageReceived, this,
                [this](const QString& sessionId, const QString& messageId)
                {
                    if (messageApplicationService == nullptr)
                    {
                        qDebug() << "RightStack::messageReceived: messageApplicationService is null";
                        return;
                    }


                    // 异步查找消息然后更新对应的MessagePanel
                    QtConcurrent::run([this, messageId]()-> std::optional<contract::message::MessageView>
                    {
                        return messageApplicationService->getMessage(messageId);
                    }).then(this, [this, messageId, sessionId](
                            const std::optional<contract::message::MessageView>& messageViewOpt)
                            {
                                if (!messageViewOpt.has_value())
                                {
                                    qDebug() << "RightStack::messageReceived: Failed to get message for id:" <<
                                        messageId;
                                    return;
                                }

                                common_widgets::ToastWidget::showToast(this, "收到新消息");
                                common_widgets::SoundPlayer::play();

                                auto messagePanel = getMessagePanel(sessionId);
                                if (messagePanel == nullptr)
                                {
                                    qDebug() << "RightStack::messageSent: 未找到对应的MessagePanel，chatSessionId=" <<
                                        sessionId;
                                    return;
                                }
                                auto messageListWidget = findMessageListWidgetInMessagePanel(messagePanel);

                                messageListWidget->addMessages({messageViewOpt.value()});
                            });
                });
    }

    message_widgets::MessagePanel* RightStack::getMessagePanel(const QString& chatSessionId)
    {
        if (chatSessionIdToPageIndex.contains(chatSessionId))
        {
            int index = chatSessionIdToPageIndex.value(chatSessionId);
            return qobject_cast<message_widgets::MessagePanel*>(widget(index));
        }
        return nullptr;
    }

    message_widgets::MessageListWidget* RightStack::findMessageListWidgetInMessagePanel(
        message_widgets::MessagePanel* messagePanel)
    {
        if (messagePanel == nullptr)
        {
            qDebug() << "RightStack::messageSent: 未找到对应的MessagePanel";
            return nullptr;
        }
        auto messageListWidget = messagePanel->findChild<message_widgets::MessageListWidget*>();
        if (messageListWidget == nullptr)
        {
            qDebug() << "RightStack::messageSent: 未找到MessagePanel中的MessageListWidget";
        }
        return messageListWidget;
    }
} // namespace ui::main_widgets

