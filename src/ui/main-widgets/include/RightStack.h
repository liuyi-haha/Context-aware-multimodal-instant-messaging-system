#pragma once
#include <QStackedWidget>

#include "ui/message-widgets/include/MessagePanel.h"

namespace contract::relation
{
    class FriendApplicationView;
}

namespace ui::relation_widgets
{
    class FriendApplicationDetail;
}

namespace ui::main_widgets
{
    class RightStack : public QStackedWidget
    {
        Q_OBJECT

    public:
        enum PageIndex
        {
            ChatPage = 0,
            FriendApplicationDetailPage = 1,
            EmptyPage = 2
        };

        explicit RightStack(QWidget* parent = nullptr);
        void showFriendApplicationDetail(const contract::relation::FriendApplicationView& friendApplicationView);

    private:
        void setupConnections();
        message_widgets::MessagePanel* getMessagePanel(const QString& chatSessionId);
        message_widgets::MessageListWidget* findMessageListWidgetInMessagePanel(
            message_widgets::MessagePanel* messagePanel);

    private:
        ui::relation_widgets::FriendApplicationDetail* m_friendApplicationDetail = nullptr;
        QHash<QString, int> chatSessionIdToPageIndex;
        sys::message::application::MessageApplicationService* messageApplicationService = QInjection::Inject;
    };
} // namespace ui::main_widgets
