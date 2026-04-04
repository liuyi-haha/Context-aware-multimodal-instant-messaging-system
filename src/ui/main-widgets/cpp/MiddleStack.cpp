#include "ui/main-widgets/include/MiddleStack.h"

#include "ui/relation-widgets/include/FriendApplicationList.h"

#include <QWidget>

#include "ui/common/UIEVentBus.h"

namespace ui::main_widgets
{
    void MiddleStack::initStyle()
    {
    }

    void MiddleStack::createPages()
    {
        auto* robotPlaceholder = new QWidget(this);


        m_chatSessionList = new ui::relation_widgets::ChatSessionList(this);
        m_friendApplicationList = new ui::relation_widgets::FriendApplicationList(this);

        auto* friendPlaceholder = new QWidget(this);


        auto* groupPlaceholder = new QWidget(this);

        addWidget(robotPlaceholder);
        addWidget(m_chatSessionList); // index : MessagePage
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

