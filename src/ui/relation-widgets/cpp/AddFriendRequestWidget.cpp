//
// Created by 86150 on 2026/4/8.
//

#include "../include/AddFriendRequestWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

#include "sys/relation-context/application/service/include/RelationApplicationService.h"
#include "ui/common-widgets/ToastWidget.h"
#include "ui/common/UIEVentBus.h"

namespace ui::relation_widgets
{
    AddFriendRequestWidget::AddFriendRequestWidget(const QString& nickname, const QString& toUserId, QWidget* parent)
        : QWidget(parent), m_toUserId(toUserId)
    {
        setWindowTitle("添加好友");
        resize(400, 300);

        // 设置背景色
        setStyleSheet(
            "AddFriendRequestWidget { background-color: rgb(247, 247, 247); }"
            "QLineEdit { background-color: white; }" // 明确设置子控件背景
            "QTextEdit { background-color: white; }"
        );

        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        // 标题
        QLabel* titleLabel = new QLabel("发送好友申请");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px; background-color: transparent;");
        mainLayout->addWidget(titleLabel);

        // 表单区域
        QFormLayout* formLayout = new QFormLayout();
        formLayout->setLabelAlignment(Qt::AlignRight);

        // 备注输入
        m_remarkEdit = new QLineEdit(nickname);
        m_remarkEdit->setPlaceholderText("请输入备注名");
        m_remarkEdit->setStyleSheet(
            "QLineEdit { padding: 5px; border: 1px solid #ccc; border-radius: 3px; background-color: white; }");
        formLayout->addRow("备注:", m_remarkEdit);

        // 验证消息输入
        m_messageEdit = new QTextEdit();
        m_messageEdit->setPlaceholderText("请输入验证消息...");
        m_messageEdit->setMaximumHeight(100);
        m_messageEdit->setStyleSheet(
            "QTextEdit { padding: 5px; border: 1px solid #ccc; border-radius: 3px; background-color: white; }");
        formLayout->addRow("验证消息:", m_messageEdit);

        mainLayout->addLayout(formLayout);

        // 按钮区域
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch(); // 左侧弹性空间

        cancelButton = new QPushButton("取消");
        sendButton = new QPushButton("发送");

        // 设置按钮样式
        sendButton->setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: white; padding: 8px 16px; border-radius: 4px; border: none; }"
            "QPushButton:hover { background-color: #45a049; }"
            "QPushButton:disabled { background-color: #cccccc; }");

        cancelButton->setStyleSheet(
            "QPushButton { background-color: #f44336; color: white; padding: 8px 16px; border-radius: 4px; border: none; }"
            "QPushButton:hover { background-color: #da190b; }");

        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(sendButton);

        mainLayout->addStretch(); // 底部弹性空间
        mainLayout->addLayout(buttonLayout);

        // 设置表单标签样式
        QString labelStyle = "QLabel { background-color: transparent; }";
        QList<QLabel*> labels = findChildren<QLabel*>();
        for (QLabel* label : labels)
        {
            label->setStyleSheet(labelStyle);
        }

        // 连接信号槽
        connect(sendButton, &QPushButton::clicked, this, &AddFriendRequestWidget::onSendClicked);
        connect(cancelButton, &QPushButton::clicked, this, &AddFriendRequestWidget::close);
    }

    void AddFriendRequestWidget::onSendClicked()
    {
        QString remark = m_remarkEdit->text().trimmed();
        QString message = m_messageEdit->toPlainText().trimmed();

        if (remark.isEmpty())
        {
            QMessageBox::warning(this, "警告", "请填写备注名！");
            return;
        }
        if (message.isEmpty())
        {
            QMessageBox::warning(this, "警告", "请填写验证消息！");
            return;
        }
        // 将按不可用

        sendButton->setText("发送中...");
        sendButton->setEnabled(false);

        QtConcurrent::run([this, remark, message]()
        {
            contract::relation::SendFriendApplicationRequest request;
            request.targetUserId = m_toUserId;
            request.verificationMessage = message;
            request.recipientRemark = remark;
            request.verificationMessage = message;
            auto response = relationApplicationService->sendFriendApplication(request);
            return response;
        }).then(this, [this](const contract::relation::SendFriendApplicationResponse& response)
        {
            // 恢复按钮状态
            sendButton->setText("发送");
            sendButton->setEnabled(true);


            if (response.success)
            {
                common_widgets::ToastWidget::showToast(this, "好友申请已发送");
                this->deleteLater();
                // 通知相关UI更新界面, 这里的UI有：好友申请列表
                common::UIEventBus::instance()->emit friendApplicationSent(response.friendApplicationId);
                return;
            }
            common_widgets::ToastWidget::showToast(this, "好友申请发送失败" + response.errMsg.value_or(""));
        });

        // emit sendRequestClicked(remark, message);
        //
        // // 发送成功后关闭窗口或清空表单
        // QMessageBox::information(this, "成功", "好友申请已发送！");
    }
}
