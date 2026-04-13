//
// Created by 86150 on 2026/3/23.
//

#include "../include/UserIdDisplayWidget.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "ui/common/UIEVentBus.h"

namespace ui::user_widgets
{
    UserIdDisplayWidget::UserIdDisplayWidget(const QString& userId, QWidget* parent)
        : QWidget(parent),
          m_userId(userId),
          m_userIdLabel(nullptr),
          m_copyButton(nullptr),
          m_goToLoginButton(nullptr)
    {
        setAttribute(Qt::WA_DeleteOnClose, true);
        createSubWidgets();
        initializeLayout();
        initializeStyle();
        setupConnections();
    }

    void UserIdDisplayWidget::createSubWidgets()
    {
        m_userIdLabel = new QLabel(m_userId, this);
        m_userIdLabel->setAlignment(Qt::AlignCenter);
        m_userIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        m_copyButton = new QPushButton(QStringLiteral("一键复制用户ID"), this);
        m_goToLoginButton = new QPushButton(QStringLiteral("去登录"), this);
    }

    void UserIdDisplayWidget::initializeLayout()
    {
        auto* rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(28, 28, 28, 28);
        rootLayout->setSpacing(20);

        auto* title = new QLabel(QStringLiteral("注册成功"), this);
        title->setObjectName(QStringLiteral("successTitle"));
        title->setAlignment(Qt::AlignCenter);

        auto* tip = new QLabel(QStringLiteral("这是您的用户ID，请妥善保存"), this);
        tip->setObjectName(QStringLiteral("successTip"));
        tip->setAlignment(Qt::AlignCenter);

        auto* buttonRow = new QHBoxLayout();
        buttonRow->addStretch(1);
        buttonRow->addWidget(m_copyButton);
        buttonRow->addSpacing(12);
        buttonRow->addWidget(m_goToLoginButton);
        buttonRow->addStretch(1);

        rootLayout->addStretch(1);
        rootLayout->addWidget(title);
        rootLayout->addWidget(tip);
        rootLayout->addWidget(m_userIdLabel);
        rootLayout->addLayout(buttonRow);
        rootLayout->addStretch(1);
    }

    void UserIdDisplayWidget::initializeStyle()
    {
        setWindowTitle(QStringLiteral("用户ID"));
        resize(520, 360);

        setStyleSheet(
            "QWidget {"
            " background-color: #f5f5f5;"
            " color: #1f1f1f;"
            "}"
            "QLabel#successTitle {"
            " font-size: 30px;"
            " font-weight: 700;"
            "}"
            "QLabel#successTip {"
            " font-size: 14px;"
            " color: #5a5a5a;"
            "}"
            "QPushButton {"
            " border: none;"
            " border-radius: 8px;"
            " padding: 8px 14px;"
            " min-height: 36px;"
            " color: #ffffff;"
            "}"
        );

        m_userIdLabel->setStyleSheet(
            "QLabel {"
            " background-color: #ffffff;"
            " border: 2px solid #07c160;"
            " border-radius: 10px;"
            " padding: 14px;"
            " font-size: 34px;"
            " font-weight: 700;"
            " color: #0b7b3d;"
            " letter-spacing: 1px;"
            "}"
        );

        m_copyButton->setStyleSheet(
            "QPushButton { background-color: #07c160; }"
            "QPushButton:hover { background-color: #06b257; }"
        );

        m_goToLoginButton->setStyleSheet(
            "QPushButton { background-color: #4e5969; }"
            "QPushButton:hover { background-color: #3e4755; }"
        );
    }

    void UserIdDisplayWidget::setupConnections()
    {
        connect(m_copyButton, &QPushButton::clicked, this, &UserIdDisplayWidget::copyUserId);
        connect(m_goToLoginButton, &QPushButton::clicked, this, [this]
        {
            emit ui::common::UIEventBus::instance()->goToLoginRequested();
            this->deleteLater();
        });
    }

    void UserIdDisplayWidget::copyUserId()
    {
        QGuiApplication::clipboard()->setText(m_userId);
        m_copyButton->setText(QStringLiteral("已复制"));
    }
} // namespace ui::user_widgets

