#pragma once
#include <QStackedWidget>

#include "ui/relation-widgets/include/ChatSessionList.h"

namespace contract::relation
{
    class FriendApplicationView;
}

namespace ui::relation_widgets
{
    class FriendApplicationList;
}

namespace ui::main_widgets
{
    class MiddleStack : public QStackedWidget
    {
        Q_OBJECT

    public:
        enum PageIndex
        {
            RobotPage = 0,
            MessagePage = 1,
            FriendApplicationPage = 2,
            FriendPage = 3,
            GroupPage = 4
        };

        void initStyle();
        void createPages();
        explicit MiddleStack(QWidget* parent = nullptr);

    private:
        void setupConnections();

    private:
        ui::relation_widgets::FriendApplicationList* m_friendApplicationList;
        ui::relation_widgets::ChatSessionList* m_chatSessionList;
    };
} // namespace ui::main_widgets
