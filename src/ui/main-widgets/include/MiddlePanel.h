//
// Created by 86150 on 2026/4/3.
//

#pragma once
#include "ui/main-widgets/include/MiddleStack.h"
#include "ui/main-widgets/include/NavBar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QAction>

#include "ui/user-widgets/include/SearchWindow.h"

namespace ui::main_widgets
{
    // MiddlePanel 类定义
    class MiddlePanel : public QWidget
    {
        Q_OBJECT

    private:
        QWidget* m_topBar;
        QLineEdit* m_searchBox;
        QPushButton* m_createGroupBtn;
        MiddleStack* m_middleStack;

    public:
        explicit MiddlePanel(QWidget* parent = nullptr)
            : QWidget(parent)
        {
            initStyle();
            createTopBar();
            createMiddleStack();
            initializeLayout();
            setupConnections();
        }

    private:
        void initStyle()
        {
            setMinimumWidth(300);
            setMaximumWidth(500);
            setStyleSheet(R"(
                QWidget {
                    background-color: rgb(247, 247, 247);
                }
            )");
        }

        void createTopBar()
        {
            m_topBar = new QWidget();
            m_topBar->setFixedHeight(50);

            QHBoxLayout* topLayout = new QHBoxLayout(m_topBar);
            topLayout->setContentsMargins(10, 0, 10, 0);
            topLayout->setSpacing(8);

            m_searchBox = new QLineEdit();
            m_searchBox->setPlaceholderText("点击搜索用户");
            m_searchBox->setStyleSheet(R"(
                QLineEdit {
                    padding: 6px 10px;
                    border: none;
                    border-radius: 4px;
                    background: rgb(234, 234, 234);
                    font-size: 12px;
                }
                QLineEdit:focus {
                    border: none;
                }
            )");

            QAction* searchAction = m_searchBox->addAction(QIcon(":icons/user/search.png"), QLineEdit::LeadingPosition);

            m_createGroupBtn = new QPushButton();
            m_createGroupBtn->setFixedSize(32, 32);
            m_createGroupBtn->setToolTip("创建群聊");
            m_createGroupBtn->setStyleSheet(R"(
                QPushButton {
                    border: none;
                    border-radius: 4px;
                    background: rgb(234, 234, 234);
                }
                QPushButton:hover {
                    background: rgb(220, 220, 220);
                }
                QPushButton:pressed {
                    background: rgb(200, 200, 200);
                }
            )");

            m_createGroupBtn->setIcon(QIcon(":icons/relation/create_group.png"));
            m_createGroupBtn->setIconSize(QSize(20, 20));

            topLayout->addWidget(m_searchBox, 1);
            topLayout->addWidget(m_createGroupBtn);
        }

        void createMiddleStack()
        {
            m_middleStack = new MiddleStack();
        }

        void initializeLayout()
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            mainLayout->setSpacing(0);

            mainLayout->addWidget(m_topBar);
            mainLayout->addWidget(m_middleStack);
        }

        void setupConnections()
        {
            // 当搜索框文本变化时弹出搜索窗口

            connect(m_searchBox, &QLineEdit::textChanged, this, [this](const QString& text)
            {
                if (!text.isEmpty())
                {
                    auto* searchWindow = new user_widgets::SearchWindow(this->window());
                    searchWindow->setAttribute(Qt::WA_DeleteOnClose);
                    searchWindow->setModal(true); // 模态显示，阻塞其他窗口
                    searchWindow->show();

                    // 直接设置搜索框文本并触发搜索
                    searchWindow->setSearchText(text);

                    m_searchBox->clear();
                }
            });
        }


        // TODO: 连接创建群聊按钮的信号槽
        // connect(m_createGroupBtn, &QPushButton::clicked, this, &MiddlePanel::onCreateGroupClicked);
    };
};

