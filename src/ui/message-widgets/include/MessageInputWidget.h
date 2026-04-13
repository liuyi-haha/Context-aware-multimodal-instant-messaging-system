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
#include "sys/message-context/application/include/MessageApplicationService.h"
#include "ui/common-widgets/ToastWidget.h"
#include "ui/common/UIEVentBus.h"
#include <QtConcurrent/QtConcurrent>

namespace ui::message_widgets
{
    class MessageInputWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit MessageInputWidget(const QString& chatSessionId, QWidget* parent = nullptr)
            : currentChatSessionId(chatSessionId),
              QWidget(parent)
        {
            setupMainLayout();
            setupToolButtons();
            setupTextEdit();
            setupSendButton();
            setupConnections();
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
            QString text = m_textEdit->toPlainText().trimmed();
            if (text.isEmpty())
            {
                common_widgets::ToastWidget::showToast(this, "消息不能为空");
                return;
            }

            m_sendBtn->setEnabled(false); // 发送后禁用按钮，防止重复点击
            m_sendBtn->setText("发送中...");
            QtConcurrent::run([this, text]()-> contract::message::SendTextMessageResponse
            {
                if (messageApplicationService == nullptr)
                {
                    qDebug() << "MessageInputWidget::onSendClicked: messageApplicationService is null";
                    return {};
                }
                qDebug() << "MessageInputWidget::onSendClicked: 开始发送消息，chatSessionId=" << currentChatSessionId
                    << ", text=" << text;
                return messageApplicationService->sendTextMessage(currentChatSessionId, text);
            }).then(this, [this](const contract::message::SendTextMessageResponse& resp)
            {
                m_sendBtn->setEnabled(true); // 发送失败，重新启用按钮
                m_sendBtn->setText("发送");
                if (!resp.success || !resp.messageView.has_value())
                {
                    common_widgets::ToastWidget::showToast(this, "消息发送失败: " + resp.errMsg.value_or("未知错误"));
                    return;
                }
                // 发送成功，清空输入框
                m_textEdit->clear();
                common_widgets::ToastWidget::showToast(this, "发送成功");
                qDebug() << "MessageInput Say: 发送消息成功，并发送messageReceived信号"; // 发送信号，通知UI更新消息列表
                emit common::UIEventBus::instance()->messageSent(resp.messageView.value(), currentChatSessionId);
                qDebug() << "MessageInput Say: 发送消息成功，并发送chatSessionUpdated信号"; // 发送信号，通知UI更新消息列表
                emit common::UIEventBus::instance()->chatSessionUpdated(currentChatSessionId);
                // 发送信号，通知ChatSessionList更新会话列表
            });
        }

    private:
        void setupMainLayout()
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            mainLayout->setSpacing(0); // 移除行间距，消除分界线
        }

        void setupToolButtons()
        {
            // 获取主布局
            QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());

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

            // 创建语音按钮
            m_voiceBtn = new QPushButton(this);
            m_voiceBtn->setToolTip("发送语音");
            m_voiceBtn->setFixedSize(28, 28);
            m_voiceBtn->setIcon(QIcon(":/icons/message/voice.png"));
            m_voiceBtn->setIconSize(QSize(20, 20));
            m_voiceBtn->setStyleSheet(flatButtonStyle);
            m_voiceBtn->setCursor(Qt::PointingHandCursor);
            m_voiceBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // 创建图片按钮
            m_imageBtn = new QPushButton(this);
            m_imageBtn->setToolTip("发送图片");
            m_imageBtn->setFixedSize(28, 28);
            m_imageBtn->setIcon(QIcon(":/icons/message/photo.png"));
            m_imageBtn->setIconSize(QSize(20, 20));
            m_imageBtn->setStyleSheet(flatButtonStyle);
            m_imageBtn->setCursor(Qt::PointingHandCursor);
            m_imageBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // 创建文件按钮
            m_fileBtn = new QPushButton(this);
            m_fileBtn->setToolTip("发送文件");
            m_fileBtn->setFixedSize(28, 28);
            m_fileBtn->setIcon(QIcon(":/icons/message/document.png"));
            m_fileBtn->setIconSize(QSize(20, 20));
            m_fileBtn->setStyleSheet(flatButtonStyle);
            m_fileBtn->setCursor(Qt::PointingHandCursor);
            m_fileBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // 创建智能回复按钮
            m_smartReplyBtn = new QPushButton(this);
            m_smartReplyBtn->setToolTip("智能回复");
            m_smartReplyBtn->setFixedSize(28, 28);
            m_smartReplyBtn->setIcon(QIcon(":/icons/message/smart_reply.png"));
            m_smartReplyBtn->setIconSize(QSize(20, 20));
            m_smartReplyBtn->setStyleSheet(flatButtonStyle);
            m_smartReplyBtn->setCursor(Qt::PointingHandCursor);
            m_smartReplyBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // 添加按钮到工具栏
            toolLayout->addWidget(m_voiceBtn);
            toolLayout->addWidget(m_imageBtn);
            toolLayout->addWidget(m_fileBtn);
            toolLayout->addWidget(m_smartReplyBtn);
            toolLayout->addStretch();

            // 将工具栏添加到主布局
            mainLayout->addLayout(toolLayout);
        }

        void setupTextEdit()
        {
            // 获取主布局
            QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());

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

            // 将文本编辑框添加到主布局
            mainLayout->addWidget(m_textEdit);
        }

        void setupSendButton()
        {
            // 获取主布局
            QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());
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
            mainLayout->addLayout(sendLayout);

            // 确保整个控件不会产生横向滚动条
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }

        void setupConnections()
        {
            // 监听文本变化
            connect(m_textEdit, &QTextEdit::textChanged, this, &MessageInputWidget::onTextChanged);

            // 连接发送按钮的点击信号
            connect(m_sendBtn, &QPushButton::clicked, this, &MessageInputWidget::onSendClicked);
        }

    private:
        QPushButton* m_voiceBtn;
        QPushButton* m_imageBtn;
        QPushButton* m_fileBtn;
        QPushButton* m_smartReplyBtn;
        QPushButton* m_sendBtn;
        QTextEdit* m_textEdit;
        QString currentChatSessionId; // 当前聊天会话ID，发送消息时需要用到
        sys::message::application::MessageApplicationService* messageApplicationService = QInjection::Inject;
    };
}
