#pragma once
#include <QStackedWidget>

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

    private:
        ui::relation_widgets::FriendApplicationDetail* m_friendApplicationDetail = nullptr;
        QHash<QString, int> chatSessionIdToPageIndex;
    };
} // namespace ui::main_widgets
