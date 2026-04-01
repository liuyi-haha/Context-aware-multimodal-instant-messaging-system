//
// Created by 86150 on 2026/3/23.
//

#include "../include/LoginWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "dependencyinjector.h"
#include "contract/system-provider/auth-context-provider/Login.h"
#include "sys/auth-context/application/service/include/AuthApplicationService.h"
#include "sys/notification-context/application/service/include/NotificationApplicationService.h"
#include "ui/user-widgets/include/RegisterWidget.h"
#include "ui/common/UIEVentBus.h"

namespace ui::auth_widgets
{
    LoginWidget::LoginWidget(
        QWidget* parent,
        sys::auth::application::AuthApplicationService* authApplicationService,
        sys::notification::application::NotificationApplicationService* notificationApplicationService)
        : QWidget(parent),
          m_titleLabel(nullptr),
          m_phoneEdit(nullptr),
          m_passwordEdit(nullptr),
          m_loginButton(nullptr),
          m_goToRegisterLink(nullptr),
          m_loginLoadingTimer(new QTimer(this)),
          m_isSubmitting(false),
          m_loadingFrameIndex(0),
          m_authApplicationService(authApplicationService),
          m_notificationApplicationService(notificationApplicationService)
    {
        setAttribute(Qt::WA_DeleteOnClose, true);
        createSubWidgets();
        initializeLayout();
        initializeStyle();
        setupConnections();
    }

    void LoginWidget::createSubWidgets()
    {
        m_titleLabel = new QLabel(QStringLiteral("登录"), this);
        m_phoneEdit = new QLineEdit(this);
        m_passwordEdit = new QLineEdit(this);
        m_loginButton = new QPushButton(QStringLiteral("登录"), this);
        m_goToRegisterLink = new QPushButton(QStringLiteral("没有账号？去注册"), this);

        m_phoneEdit->setPlaceholderText(QStringLiteral("请输入手机号或用户ID"));
        m_passwordEdit->setPlaceholderText(QStringLiteral("请输入密码"));
        m_passwordEdit->setEchoMode(QLineEdit::Password);
    }

    void LoginWidget::initializeLayout()
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(32, 32, 32, 24);
        layout->setSpacing(12);

        layout->addWidget(m_titleLabel);
        layout->addWidget(m_phoneEdit);
        layout->addWidget(m_passwordEdit);
        layout->addWidget(m_loginButton);
        layout->addStretch(1);

        auto* bottomRow = new QHBoxLayout();
        bottomRow->setContentsMargins(0, 0, 0, 0);
        bottomRow->addWidget(m_goToRegisterLink, 0, Qt::AlignLeft | Qt::AlignBottom);
        bottomRow->addStretch(1);
        layout->addLayout(bottomRow);
    }

    void LoginWidget::initializeStyle()
    {
        setWindowTitle(QStringLiteral("登录页面"));
        resize(420, 320);

        m_titleLabel->setAlignment(Qt::AlignCenter);
        m_titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: 700; }");

        m_goToRegisterLink->setCursor(Qt::PointingHandCursor);
        m_goToRegisterLink->setFlat(true);
        m_goToRegisterLink->setStyleSheet(
            "QPushButton {"
            " border: none;"
            " background: transparent;"
            " color: #1677ff;"
            " font-size: 12px;"
            " padding: 0;"
            " text-align: left;"
            "}"
            "QPushButton:hover { color: #4096ff; text-decoration: underline; }"
        );

        setStyleSheet(
            "QWidget { background-color: #f5f5f5; color: #1f1f1f; }"
            "QLineEdit {"
            " background-color: #ffffff;"
            " border: 1px solid #dcdcdc;"
            " border-radius: 8px;"
            " padding: 8px 10px;"
            " min-height: 20px;"
            "}"
            "QPushButton {"
            " border: none;"
            " border-radius: 8px;"
            " padding: 8px 14px;"
            " background-color: #07c160;"
            " color: #ffffff;"
            " min-height: 36px;"
            "}"
            "QPushButton:hover { background-color: #06b257; }"
        );
    }

    void LoginWidget::setupConnections()
    {
        connect(m_loginButton, &QPushButton::clicked, this, &LoginWidget::submitLogin);
        connect(m_goToRegisterLink, &QPushButton::clicked, this, &LoginWidget::goToRegister);
        connect(m_loginLoadingTimer, &QTimer::timeout, this, &LoginWidget::tickLoginLoading);

        connect(ui::common::UIEventBus::instance(), &ui::common::UIEventBus::goToRegisterRequested, this, [this]
        {
            auto* registerWidget = new ui::user_widgets::RegisterWidget(nullptr);
            registerWidget->setAttribute(Qt::WA_DeleteOnClose, true);
            registerWidget->show();
            close();
        });
    }

    void LoginWidget::setSubmitting(bool submitting)
    {
        m_isSubmitting = submitting;

        m_phoneEdit->setEnabled(!submitting);
        m_passwordEdit->setEnabled(!submitting);
        m_goToRegisterLink->setEnabled(!submitting);

        if (submitting)
        {
            m_loadingFrameIndex = 0;
            m_loginLoadingTimer->start(120);
            tickLoginLoading();
        }
        else
        {
            m_loginLoadingTimer->stop();
            m_loginButton->setText(QStringLiteral("登录"));
            m_loginButton->setEnabled(true);
        }
    }

    void LoginWidget::tickLoginLoading()
    {
        static const char* frames[] = {"|", "/", "-", "\\"};
        const QString frame = QString::fromLatin1(frames[m_loadingFrameIndex % 4]);
        ++m_loadingFrameIndex;
        m_loginButton->setText(QStringLiteral("登录中 ") + frame);
        m_loginButton->setEnabled(false);
    }

    void LoginWidget::submitLogin()
    {
        if (m_isSubmitting)
        {
            return;
        }

        const QString account = m_phoneEdit->text().trimmed();
        const QString password = m_passwordEdit->text();

        if (account.isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("校验失败"), QStringLiteral("请输入手机号或用户ID。"));
            return;
        }
        if (password.isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("校验失败"), QStringLiteral("请输入密码。"));
            return;
        }

        auto* appService = m_authApplicationService;
        if (appService == nullptr)
        {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("登录服务不可用，请稍后重试。"));
            return;
        }

        contract::auth::LoginRequest request{};
        request.account = account;
        request.password = password;

        setSubmitting(true);

        QtConcurrent::run([appService, notificationApplicationService = m_notificationApplicationService, request]()
        {
            const auto response = appService->login(request);
            if (response.success && notificationApplicationService != nullptr)
            {
                notificationApplicationService->tryToConnect();
            }
            return response;
        }).then(this, [this](const contract::auth::LoginResponse& response)
        {
            setSubmitting(false);
            if (!response.success)
            {
                QMessageBox::warning(this, QStringLiteral("登录失败"), response.errMsg.value_or(QStringLiteral("未知错误")));
                return;
            }

            QMessageBox::information(this, QStringLiteral("登录成功"), QStringLiteral("登录成功，用户ID：") + response.userId);
        });
    }

    void LoginWidget::goToRegister()
    {
        if (m_isSubmitting)
        {
            return;
        }

        emit ui::common::UIEventBus::instance()->goToRegisterRequested();
    }
} // auth_widgets

