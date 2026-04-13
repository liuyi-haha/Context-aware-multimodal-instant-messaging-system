//
// Created by 86150 on 2026/3/31.
//
#pragma once
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include "ChatSessionTitleWidget.h"
#include "MessageInputWidget.h"
#include "MessageList.h"

namespace ui::message_widgets
{
    class MessagePanel : public QWidget
    {
        Q_OBJECT

    public:
        explicit MessagePanel(const QString& chatSessionId, const QString& chatSessionName, QWidget* parent = nullptr)
            : m_chatSessionId(chatSessionId),
              m_chatSessionName(chatSessionName),
              QWidget(parent)
        {
            m_titleBar = new ChatSessionTitleWidget(chatSessionName, this);
            m_inputWidget = new MessageInputWidget(m_chatSessionId, this);
            m_messageList = new MessageListWidget(m_chatSessionId, this);
            initLayout();
            initStyle();
        }

    private:
        void initStyle()
        {
            setStyleSheet(
                "MessagePanel {"
                " border: none;"
                "}"
                " #chatSessionTitleWidget, MessageListWidget, MessageInputWidget {"
                " background-color: rgb(237, 237, 237);"
                " border: none;"
                " outline: none;"
                "}"
                "QScrollBar:vertical {"
                " background: transparent;"
                " width: 6px;"
                " margin: 4px 2px 4px 0px;"
                "}"
                "QScrollBar::handle:vertical {"
                " background: rgba(0, 0, 0, 45);"
                " border-radius: 3px;"
                " min-height: 20px;"
                "}"
                "QScrollBar::handle:vertical:hover {"
                " background: rgba(0, 0, 0, 70);"
                "}"
                "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,"
                "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                " background: transparent;"
                " height: 0px;"
                "}"
            );
        }

        void initLayout()
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            mainLayout->setSpacing(0);

            // 使用 QSplitter 控制消息列表和输入框
            m_splitter = new QSplitter(Qt::Vertical, this);

            m_splitter->addWidget(m_messageList);
            m_splitter->addWidget(m_inputWidget);

            // 设置初始比例：消息列表占 70%，输入框固定高度
            m_splitter->setStretchFactor(0, 7);
            m_splitter->setStretchFactor(1, 0);

            // 设置输入框最小/最大高度
            m_inputWidget->setMinimumHeight(100);
            m_inputWidget->setMaximumHeight(400);

            // 允许用户拖拽调节
            m_splitter->setChildrenCollapsible(false);
            mainLayout->addWidget(m_titleBar);
            mainLayout->addWidget(m_splitter, 1);
        }

    private:
        ChatSessionTitleWidget* m_titleBar;
        QSplitter* m_splitter;
        MessageListWidget* m_messageList;
        MessageInputWidget* m_inputWidget;
        QString m_chatSessionId;
        QString m_chatSessionName;
    };
}
