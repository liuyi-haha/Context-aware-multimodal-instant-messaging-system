#include "ui/main-widgets/include/NavBar.h"

#include <QIcon>
#include <QPushButton>
#include <QSize>
#include <QVBoxLayout>

#include "ui/common/UIEVentBus.h"

namespace ui::main_widgets
{
    void NavBar::initStyle()
    {
        setFixedWidth(60);
        setAttribute(Qt::WA_StyledBackground, true);
        setStyleSheet("background-color: rgb(231, 231, 231);");
    }

    void NavBar::createSubWidgets()
    {
        m_btnRobot = createNavButton(":/icons/smart/chat_robot.png", "聊天机器人", 0);
        m_btnMessage = createNavButton(":/icons/message/message_nav.png", "消息", 1);
        m_btnFriendApplication = createNavButton(":/icons/relation/friend_application_nav.png", "好友申请", 2);
        m_btnFriend = createNavButton(":/icons/relation/friend_nav.png", "好友", 3);
        m_btnGroup = createNavButton(":/icons/relation/group_nav.png", "群聊", 4);
    }

    void NavBar::initLayout()
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(8, 14, 8, 14);
        layout->setSpacing(6);
        layout->QLayoutItem::setAlignment(Qt::AlignTop);
        layout->addWidget(m_btnRobot);
        layout->addWidget(m_btnMessage);
        layout->addWidget(m_btnFriendApplication);
        layout->addWidget(m_btnFriend);
        layout->addWidget(m_btnGroup);
        layout->addStretch();
    }

    QString statusToText(common::ConnectionStatus status)
    {
        if (status == common::ConnectionStatus::Connected)
        {
            return "已连接";
        }
        if (status == common::ConnectionStatus::Disconnected)
        {
            return "已断开";
        }
        if (status == common::ConnectionStatus::Connecting)
        {
            return "连接中";
        }
        return "未知状态";
    }

    void NavBar::setupConnections()
    {
        // 订阅连接状态改变信号
        connect(common::UIEventBus::instance(), &common::UIEventBus::connectionStatusChanged, this,
                [this](ui::common::ConnectionStatus status)
                {
                    // todo @liuyi 待完善
                    qDebug() << "NavBar say: " << "I received connection status changed signal, new status: " <<
                        statusToText(status);
                });
    }

    NavBar::NavBar(QWidget* parent)
        : QWidget(parent),
          m_btnRobot(nullptr),
          m_btnMessage(nullptr),
          m_btnFriendApplication(nullptr),
          m_btnFriend(nullptr),
          m_btnGroup(nullptr)
    {
        initStyle();
        createSubWidgets();
        initLayout();
        setActiveButton(m_btnMessage);
        setupConnections();
    }

    QPushButton* NavBar::createNavButton(const QString& iconPath, const QString& tooltip, int index)
    {
        auto* btn = new QPushButton();
        btn->setToolTip(tooltip);
        btn->setFixedSize(44, 44);
        btn->setCheckable(true);
        btn->setProperty("navIndex", index);
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(QSize(22, 22));
        btn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: 8px;
        }
        QPushButton:hover {
            background-color: #dcdcdc;
        }
        QPushButton:checked {
            background-color: #07c160;
        }
    )");

        connect(btn, &QPushButton::clicked, this, [this, btn]()
        {
            setActiveButton(btn);
            emit ui::common::UIEventBus::instance()->navSelected(btn->property("navIndex").toInt());
        });

        return btn;
    }

    void NavBar::setActiveButton(QPushButton* activeButton)
    {
        if (m_btnRobot)
        {
            m_btnRobot->setChecked(m_btnRobot == activeButton);
        }
        if (m_btnMessage)
        {
            m_btnMessage->setChecked(m_btnMessage == activeButton);
        }
        if (m_btnFriendApplication)
        {
            m_btnFriendApplication->setChecked(m_btnFriendApplication == activeButton);
        }
        if (m_btnFriend)
        {
            m_btnFriend->setChecked(m_btnFriend == activeButton);
        }
        if (m_btnGroup)
        {
            m_btnGroup->setChecked(m_btnGroup == activeButton);
        }
    }
} // namespace ui::main_widgets

