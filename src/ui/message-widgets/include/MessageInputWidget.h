//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>

#include "ui/auth-widgets/include/LoginWidget.h"

namespace ui::message_widgets
{
    class MessageInputWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit MessageInputWidget(QWidget* parent = nullptr)
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);

            // 工具栏
            QHBoxLayout* toolLayout = new QHBoxLayout();
            toolLayout->setContentsMargins(5, 5, 5, 5);
            toolLayout->setSpacing(10);

            m_voiceBtn = new QPushButton(this);
            m_voiceBtn->setToolTip("发送语音");
            m_voiceBtn->setFixedSize(32, 32);
            m_voiceBtn->setIcon(QIcon(":/icons/voice.png"));
            m_voiceBtn->setIconSize(QSize(24, 24));

            m_imageBtn = new QPushButton(this);
            m_imageBtn->setToolTip("发送图片");
            m_imageBtn->setFixedSize(32, 32);
            m_imageBtn->setIcon(QIcon(":/icons/image.png"));
            m_imageBtn->setIconSize(QSize(24, 24));

            m_fileBtn = new QPushButton(this);
            m_fileBtn->setToolTip("发送文件");
            m_fileBtn->setFixedSize(32, 32);
            m_fileBtn->setIcon(QIcon(":/icons/file.png"));
            m_fileBtn->setIconSize(QSize(24, 24));

            m_smartReplyBtn = new QPushButton(this);
            m_smartReplyBtn->setToolTip("智能回复");
            m_smartReplyBtn->setFixedSize(32, 32);
            m_smartReplyBtn->setIcon(QIcon(":/icons/smart_reply.png"));
            m_smartReplyBtn->setIconSize(QSize(24, 24));

            m_sendBtn = new QPushButton("发送", this);
            m_sendBtn->setFixedSize(60, 32);
            m_sendBtn->setStyleSheet(
                "QPushButton {"
                "   background-color: #07C160;"
                "   color: white;"
                "   border: none;"
                "   border-radius: 4px;"
                "   font-size: 14px;"
                "}"
                "QPushButton:hover {"
                "   background-color: #06AD56;"
                "}"
                "QPushButton:pressed {"
                "   background-color: #059A4A;"
                "}"
            );

            toolLayout->addWidget(m_voiceBtn);
            toolLayout->addWidget(m_imageBtn);
            toolLayout->addWidget(m_fileBtn);
            toolLayout->addWidget(m_smartReplyBtn);
            toolLayout->addStretch();
            toolLayout->addWidget(m_sendBtn);

            // 文本输入框
            m_textEdit = new QTextEdit(this);
            m_textEdit->setPlaceholderText("输入消息...");
            m_textEdit->setMaximumHeight(100);

            mainLayout->addLayout(toolLayout);
            mainLayout->addWidget(m_textEdit);
        }

    private:
        QPushButton* m_voiceBtn;
        QPushButton* m_imageBtn;
        QPushButton* m_fileBtn;
        QPushButton* m_smartReplyBtn;
        QPushButton* m_sendBtn;
        QTextEdit* m_textEdit;
    };
}
