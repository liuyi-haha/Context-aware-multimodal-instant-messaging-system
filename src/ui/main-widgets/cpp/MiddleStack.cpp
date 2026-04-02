#include "ui/main-widgets/include/MiddleStack.h"

#include "ui/relation-widgets/include/FriendApplicationList.h"

#include <QWidget>

#include "ui/common/UIEVentBus.h"

namespace ui::main_widgets
{
    void MiddleStack::initStyle()
    {
        setMinimumWidth(300);
        setMaximumWidth(500);
    }

    void MiddleStack::createPages()
    {
        auto* robotPlaceholder = new QWidget();
        robotPlaceholder->setStyleSheet("background-color: white;");

        m_friendApplicationList = new ui::relation_widgets::FriendApplicationList();
        m_chatSessionList = new ui::relation_widgets::ChatSessionList();
        auto* friendPlaceholder = new QWidget();
        friendPlaceholder->setStyleSheet("background-color: white;");

        auto* groupPlaceholder = new QWidget();
        groupPlaceholder->setStyleSheet("background-color: white;");

        addWidget(robotPlaceholder);
        addWidget(m_chatSessionList);
        addWidget(m_friendApplicationList);
        addWidget(friendPlaceholder);
        addWidget(groupPlaceholder);

        setCurrentIndex(MessagePage);
    }

    MiddleStack::MiddleStack(QWidget* parent)
        : QStackedWidget(parent)
    {
        initStyle();
        createPages();
        setupConnections();
    }

    void MiddleStack::setupConnections()
    {
        connect(ui::common::UIEventBus::instance(), &ui::common::UIEventBus::navSelected, this, [this](const int index)
        {
            if (index >= 0 && index < count())
            {
                setCurrentIndex(index);
            }
        });
    }
} // namespace ui::main_widgets

