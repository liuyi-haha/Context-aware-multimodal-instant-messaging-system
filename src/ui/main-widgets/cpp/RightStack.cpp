#include "ui/main-widgets/include/RightStack.h"

#include <QWidget>

#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"
#include "ui/common/UIEVentBus.h"
#include "ui/message-widgets/include/MessagePanel.h"
#include "ui/relation-widgets/include/FriendApplicationDetail.h"

namespace ui::main_widgets
{
    RightStack::RightStack(QWidget* parent)
        : QStackedWidget(parent)
    {
        setStyleSheet("background-color: white;");

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
                    if (chatSessionIdToPageIndex.contains(chatSessionView.chatSessionId))
                    {
                        auto messagePanel = new message_widgets::MessagePanel(
                            chatSessionView.chatSessionId, chatSessionView.name, this);
                        int index = addWidget(messagePanel);
                        chatSessionIdToPageIndex.insert(chatSessionView.chatSessionId, index);
                    }
                    setCurrentIndex(chatSessionIdToPageIndex.value(chatSessionView.chatSessionId, EmptyPage));
                });
    }
} // namespace ui::main_widgets

