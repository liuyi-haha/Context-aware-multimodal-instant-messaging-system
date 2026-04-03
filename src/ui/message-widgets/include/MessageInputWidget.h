//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QIcon>
#include <QScrollBar>
#include "ui/auth-widgets/include/LoginWidget.h"

namespace ui::message_widgets
{
    class MessageInputWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit MessageInputWidget(QWidget* parent = nullptr)
            : QWidget(parent)
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            mainLayout->setSpacing(0); // 移除行间距，消除分界线

            // 工具栏 - 按钮融入背景，减小高度
            QHBoxLayout* toolLayout = new QHBoxLayout();
            toolLayout->setContentsMargins(5, 2, 5, 2); // 减小上下边距
            toolLayout->setSpacing(8);

            // 统一的融入背景的按钮样式
            QString flatButtonStyle =
                "QPushButton {"
                "   background-color: transparent;"
                "   border: none;"
                "   border-radius: 4px;"
                "}"
                "QPushButton:hover {"
                "   background-color: rgba(0, 0, 0, 0.08);"
                "}"
                "QPushButton:pressed {"
                "   background-color: rgba(0, 0, 0, 0.12);"
                "}";

            m_voiceBtn = new QPushButton(this);
            m_voiceBtn->setToolTip("发送语音");
            m_voiceBtn->setFixedSize(28, 28); // 减小按钮尺寸
            m_voiceBtn->setIcon(QIcon(":/icons/message/voice.png"));
            m_voiceBtn->setIconSize(QSize(20, 20));
            m_voiceBtn->setStyleSheet(flatButtonStyle);
            m_voiceBtn->setCursor(Qt::PointingHandCursor);
            m_voiceBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            m_imageBtn = new QPushButton(this);
            m_imageBtn->setToolTip("发送图片");
            m_imageBtn->setFixedSize(28, 28);
            m_imageBtn->setIcon(QIcon(":/icons/message/photo.png"));
            m_imageBtn->setIconSize(QSize(20, 20));
            m_imageBtn->setStyleSheet(flatButtonStyle);
            m_imageBtn->setCursor(Qt::PointingHandCursor);
            m_imageBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            m_fileBtn = new QPushButton(this);
            m_fileBtn->setToolTip("发送文件");
            m_fileBtn->setFixedSize(28, 28);
            m_fileBtn->setIcon(QIcon(":/icons/message/document.png"));
            m_fileBtn->setIconSize(QSize(20, 20));
            m_fileBtn->setStyleSheet(flatButtonStyle);
            m_fileBtn->setCursor(Qt::PointingHandCursor);
            m_fileBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            m_smartReplyBtn = new QPushButton(this);
            m_smartReplyBtn->setToolTip("智能回复");
            m_smartReplyBtn->setFixedSize(28, 28);
            m_smartReplyBtn->setIcon(QIcon(":/icons/message/smart_reply.png"));
            m_smartReplyBtn->setIconSize(QSize(20, 20));
            m_smartReplyBtn->setStyleSheet(flatButtonStyle);
            m_smartReplyBtn->setCursor(Qt::PointingHandCursor);
            m_smartReplyBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            toolLayout->addWidget(m_voiceBtn);
            toolLayout->addWidget(m_imageBtn);
            toolLayout->addWidget(m_fileBtn);
            toolLayout->addWidget(m_smartReplyBtn);
            toolLayout->addStretch();

            // 文本输入框 - 防止横向滚动条
            m_textEdit = new QTextEdit(this);
            m_textEdit->setPlaceholderText("输入消息...");
            m_textEdit->setMaximumHeight(80);
            m_textEdit->setMinimumHeight(50);
            m_textEdit->setFrameShape(QFrame::NoFrame); // 移除边框

            // 关键：禁用横向滚动条，启用自动换行
            m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_textEdit->setLineWrapMode(QTextEdit::WidgetWidth); // 根据控件宽度自动换行
            m_textEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere); // 单词边界换行

            // 监听文本变化
            connect(m_textEdit, &QTextEdit::textChanged, this, &MessageInputWidget::onTextChanged);

            // 发送按钮放在下方，减小高度
            QHBoxLayout* sendLayout = new QHBoxLayout();
            sendLayout->setContentsMargins(0, 2, 8, 4); // 减小上下边距
            sendLayout->addStretch();

            m_sendBtn = new QPushButton("发送", this);
            m_sendBtn->setFixedSize(70, 32); // 减小按钮尺寸
            m_sendBtn->setCursor(Qt::PointingHandCursor);
            m_sendBtn->setEnabled(false);
            m_sendBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            m_sendBtn->setStyleSheet(
                "QPushButton {"
                "   background-color: #07C160;"
                "   color: white;"
                "   border: none;"
                "   border-radius: 4px;"
                "   font-size: 13px;"
                "   font-weight: 500;"
                "}"
                "QPushButton:hover:enabled {"
                "   background-color: #06AD56;"
                "}"
                "QPushButton:pressed:enabled {"
                "   background-color: #059A4A;"
                "}"
                "QPushButton:disabled {"
                "   background-color: #D0D0D0;"
                "   color: #8E8E93;"
                "}"
            );
            sendLayout->addWidget(m_sendBtn);

            // 连接发送按钮的点击信号
            connect(m_sendBtn, &QPushButton::clicked, this, &MessageInputWidget::onSendClicked);

            mainLayout->addLayout(toolLayout);
            mainLayout->addWidget(m_textEdit);
            mainLayout->addLayout(sendLayout);

            // 确保整个控件不会产生横向滚动条
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }

    signals:
        // 发送消息信号
        void sendMessage(const QString& message);

    private slots:
        void onTextChanged()
        {
            // 检查输入框是否有实际内容（不只是空格）
            QString text = m_textEdit->toPlainText();
            bool hasContent = !text.trimmed().isEmpty();
            m_sendBtn->setEnabled(hasContent);
        }

        void onSendClicked()
        {
            QString message = m_textEdit->toPlainText().trimmed();
            if (!message.isEmpty())
            {
                emit sendMessage(message);
                m_textEdit->clear();
                m_sendBtn->setEnabled(false);
            }
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
