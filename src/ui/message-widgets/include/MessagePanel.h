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
            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            mainLayout->setSpacing(0);

            m_titleBar = new ChatSessionTitleWidget(chatSessionName, this);

            // 使用 QSplitter 控制消息列表和输入框
            m_splitter = new QSplitter(Qt::Vertical, this);
            m_messageList = new MessageListWidget(m_splitter);
            m_inputWidget = new MessageInputWidget(m_splitter);

            m_splitter->addWidget(m_messageList);
            m_splitter->addWidget(m_inputWidget);

            // 设置初始比例：消息列表占 70%，输入框固定高度
            m_splitter->setStretchFactor(0, 7);
            m_splitter->setStretchFactor(1, 0);

            // 设置输入框最小/最大高度
            m_inputWidget->setMinimumHeight(100);
            m_inputWidget->setMaximumHeight(200);

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
