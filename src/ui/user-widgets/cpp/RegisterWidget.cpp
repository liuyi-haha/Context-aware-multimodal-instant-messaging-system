//
// Created by 86150 on 2026/3/23.
//

#include "../include/RegisterWidget.h"

#include "dependencyinjector.h"
#include "contract/system-provider/user-context-provider/Register.hpp"
#include "sys/user-context/application/service/include/UserApplicationService.h"
#include "ui/auth-widgets/include/LoginWidget.h"
#include "ui/user-widgets/include/UserIdDisplayWidget.h"
#include "ui/common/UIEVentBus.h"

#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QTimer>
#include <QUuid>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

namespace ui::user_widgets
{
    RegisterWidget::RegisterWidget(
        QWidget* parent,
        sys::user::application::UserApplicationService* userApplicationService)
        : QWidget(parent),
          m_avatarPreview(nullptr),
          m_phoneEdit(nullptr),
          m_passwordEdit(nullptr),
          m_nicknameEdit(nullptr),
          m_nicknameHintLabel(nullptr),
          m_phoneHintLabel(nullptr),
          m_passwordHintLabel(nullptr),
          m_registerButton(nullptr),
          m_goToLoginLink(nullptr),
          m_registerLoadingTimer(new QTimer(this)),
          m_isSubmitting(false),
          m_loadingFrameIndex(0),
          m_userApplicationService(userApplicationService)
    {
        setAttribute(Qt::WA_DeleteOnClose, true);
        createSubWidgets();
        initializeLayout();
        initializeStyle();
        setupConnections();
        updateValidationState();
    }

    bool RegisterWidget::eventFilter(QObject* watched, QEvent* event)
    {
        if (m_isSubmitting)
        {
            return QWidget::eventFilter(watched, event);
        }

        if (watched == m_avatarPreview && event->type() == QEvent::MouseButtonRelease)
        {
            chooseAvatar();
            return true;
        }

        return QWidget::eventFilter(watched, event);
    }

    void RegisterWidget::createSubWidgets()
    {
        m_avatarPreview = new QLabel(this);
        m_avatarPreview->setFixedSize(96, 96);
        m_avatarPreview->setAlignment(Qt::AlignCenter);
        m_avatarPreview->setText(QStringLiteral("点击上传头像"));
        m_avatarPreview->setCursor(Qt::PointingHandCursor);

        m_nicknameEdit = new QLineEdit(this);
        m_nicknameEdit->setPlaceholderText(QStringLiteral("请输入昵称"));

        m_phoneEdit = new QLineEdit(this);
        m_phoneEdit->setPlaceholderText(QStringLiteral("请输入手机号"));
        m_phoneEdit->setMaxLength(11);
        m_phoneEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]{0,11}$"), m_phoneEdit));

        m_passwordEdit = new QLineEdit(this);
        m_passwordEdit->setPlaceholderText(QStringLiteral("请输入密码"));
        m_passwordEdit->setEchoMode(QLineEdit::Password);

        m_nicknameHintLabel = new QLabel(QStringLiteral("格式：仅中英文和下划线，至少1个字符"), this);
        m_phoneHintLabel = new QLabel(QStringLiteral("格式：11位数字"), this);
        m_passwordHintLabel = new QLabel(QStringLiteral("格式：数字/中英文/下划线，至少6个字符"), this);

        m_registerButton = new QPushButton(QStringLiteral("注册"), this);
        m_goToLoginLink = new QPushButton(QStringLiteral("已有账号？去登录"), this);
    }

    void RegisterWidget::initializeLayout()
    {
        auto* rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(24, 24, 24, 24);
        rootLayout->setSpacing(16);

        auto* title = new QLabel(QStringLiteral("注册"), this);
        title->setObjectName(QStringLiteral("registerTitle"));
        title->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        rootLayout->addWidget(title);

        auto* avatarRow = new QHBoxLayout();
        avatarRow->addStretch(1);
        avatarRow->addWidget(m_avatarPreview, 0, Qt::AlignCenter);
        avatarRow->addStretch(1);
        rootLayout->addLayout(avatarRow);

        auto* formLayout = new QFormLayout();
        formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
        formLayout->setFormAlignment(Qt::AlignTop);
        formLayout->setHorizontalSpacing(12);
        formLayout->setVerticalSpacing(12);

        auto* nicknameFieldLayout = new QVBoxLayout();
        nicknameFieldLayout->setSpacing(4);
        nicknameFieldLayout->setContentsMargins(0, 0, 0, 0);
        nicknameFieldLayout->addWidget(m_nicknameEdit);
        nicknameFieldLayout->addWidget(m_nicknameHintLabel);
        auto* nicknameField = new QWidget(this);
        nicknameField->setLayout(nicknameFieldLayout);

        auto* phoneFieldLayout = new QVBoxLayout();
        phoneFieldLayout->setSpacing(4);
        phoneFieldLayout->setContentsMargins(0, 0, 0, 0);
        phoneFieldLayout->addWidget(m_phoneEdit);
        phoneFieldLayout->addWidget(m_phoneHintLabel);
        auto* phoneField = new QWidget(this);
        phoneField->setLayout(phoneFieldLayout);

        auto* passwordFieldLayout = new QVBoxLayout();
        passwordFieldLayout->setSpacing(4);
        passwordFieldLayout->setContentsMargins(0, 0, 0, 0);
        passwordFieldLayout->addWidget(m_passwordEdit);
        passwordFieldLayout->addWidget(m_passwordHintLabel);
        auto* passwordField = new QWidget(this);
        passwordField->setLayout(passwordFieldLayout);

        formLayout->addRow(QStringLiteral("昵称"), nicknameField);
        formLayout->addRow(QStringLiteral("手机号"), phoneField);
        formLayout->addRow(QStringLiteral("密码"), passwordField);

        rootLayout->addLayout(formLayout);
        rootLayout->addWidget(m_registerButton);
        rootLayout->addStretch(1);

        auto* bottomRow = new QHBoxLayout();
        bottomRow->setContentsMargins(0, 0, 0, 0);
        bottomRow->addWidget(m_goToLoginLink, 0, Qt::AlignLeft | Qt::AlignBottom);
        bottomRow->addStretch(1);
        rootLayout->addLayout(bottomRow);
    }

    void RegisterWidget::initializeStyle()
    {
        setWindowTitle(QStringLiteral("注册"));
        resize(420, 520);
        setMinimumSize(380, 480);

        setStyleSheet(
            "QWidget {"
            " background-color: #f5f5f5;"
            " color: #1f1f1f;"
            "}"
            "QLabel#registerTitle {"
            " font-size: 24px;"
            " font-weight: 700;"
            "}"
            "QLabel {"
            " font-size: 14px;"
            "}"
            "QLineEdit {"
            " background-color: #ffffff;"
            " border: 1px solid #dcdcdc;"
            " border-radius: 8px;"
            " padding: 8px 10px;"
            " min-height: 20px;"
            "}"
            "QLineEdit:focus {"
            " border: 1px solid #07c160;"
            "}"
            "QPushButton {"
            " border: none;"
            " border-radius: 8px;"
            " padding: 8px 14px;"
            " background-color: #e9e9e9;"
            "}"
            "QPushButton:hover {"
            " background-color: #dddddd;"
            "}"
        );

        m_avatarPreview->setStyleSheet(
            "QLabel {"
            " border: 1px dashed #bcbcbc;"
            " border-radius: 8px;"
            " background-color: #ffffff;"
            " color: #666666;"
            "}"
        );

        m_nicknameHintLabel->setStyleSheet("QLabel { color: #8a8a8a; font-size: 12px; }");
        m_phoneHintLabel->setStyleSheet("QLabel { color: #8a8a8a; font-size: 12px; }");
        m_passwordHintLabel->setStyleSheet("QLabel { color: #8a8a8a; font-size: 12px; }");

        m_registerButton->setStyleSheet(
            "QPushButton {"
            " background-color: #07c160;"
            " color: #ffffff;"
            " font-size: 15px;"
            " font-weight: 600;"
            " min-height: 36px;"
            "}"
            "QPushButton:hover {"
            " background-color: #06b257;"
            "}"
        );

        m_goToLoginLink->setCursor(Qt::PointingHandCursor);
        m_goToLoginLink->setFlat(true);
        m_goToLoginLink->setStyleSheet(
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
    }

    void RegisterWidget::setupConnections()
    {
        m_avatarPreview->installEventFilter(this);

        connect(m_nicknameEdit, &QLineEdit::textChanged, this, &RegisterWidget::updateValidationState);
        connect(m_phoneEdit, &QLineEdit::textChanged, this, &RegisterWidget::updateValidationState);
        connect(m_passwordEdit, &QLineEdit::textChanged, this, &RegisterWidget::updateValidationState);
        connect(m_registerButton, &QPushButton::clicked, this, &RegisterWidget::submitRegister);
        connect(m_goToLoginLink, &QPushButton::clicked, this, &RegisterWidget::goToLogin);
        connect(m_registerLoadingTimer, &QTimer::timeout, this, &RegisterWidget::tickRegisterLoading);

        connect(ui::common::UIEventBus::instance(), &ui::common::UIEventBus::goToLoginRequested, this, [this]
        {
            auto* loginWidget = new ui::auth_widgets::LoginWidget(nullptr);
            loginWidget->setAttribute(Qt::WA_DeleteOnClose, true);
            loginWidget->show();
            close();
        });
    }

    bool RegisterWidget::isNicknameValid(const QString& nickname)
    {
        static const QRegularExpression regex(QStringLiteral("^[A-Za-z_\\x{4E00}-\\x{9FFF}]+$"));
        return regex.match(nickname).hasMatch();
    }

    bool RegisterWidget::isPhoneValid(const QString& phone)
    {
        static const QRegularExpression regex(QStringLiteral("^[0-9]{11}$"));
        return regex.match(phone).hasMatch();
    }

    bool RegisterWidget::isPasswordValid(const QString& password)
    {
        static const QRegularExpression regex(QStringLiteral("^[A-Za-z0-9_\\x{4E00}-\\x{9FFF}]{6,}$"));
        return regex.match(password).hasMatch();
    }

    void RegisterWidget::updateValidationState()
    {
        const QString nickname = m_nicknameEdit->text().trimmed();
        const QString phone = m_phoneEdit->text().trimmed();
        const QString password = m_passwordEdit->text();

        const bool nicknameValid = isNicknameValid(nickname);
        const bool phoneValid = isPhoneValid(phone);
        const bool passwordValid = isPasswordValid(password);

        if (nickname.isEmpty())
        {
            m_nicknameHintLabel->setText(QStringLiteral("格式：仅中英文和下划线，至少1个字符"));
            m_nicknameHintLabel->setStyleSheet("QLabel { color: #8a8a8a; font-size: 12px; }");
        }
        else if (nicknameValid)
        {
            m_nicknameHintLabel->setText(QStringLiteral("昵称格式正确"));
            m_nicknameHintLabel->setStyleSheet("QLabel { color: #0a8f46; font-size: 12px; }");
        }
        else
        {
            m_nicknameHintLabel->setText(QStringLiteral("昵称格式错误：仅支持中英文和下划线，且至少1个字符"));
            m_nicknameHintLabel->setStyleSheet("QLabel { color: #d93025; font-size: 12px; }");
        }

        if (phone.isEmpty())
        {
            m_phoneHintLabel->setText(QStringLiteral("格式：11位数字"));
            m_phoneHintLabel->setStyleSheet("QLabel { color: #8a8a8a; font-size: 12px; }");
        }
        else if (phoneValid)
        {
            m_phoneHintLabel->setText(QStringLiteral("手机号格式正确"));
            m_phoneHintLabel->setStyleSheet("QLabel { color: #0a8f46; font-size: 12px; }");
        }
        else
        {
            m_phoneHintLabel->setText(QStringLiteral("手机号格式错误：必须为11位数字"));
            m_phoneHintLabel->setStyleSheet("QLabel { color: #d93025; font-size: 12px; }");
        }

        if (password.isEmpty())
        {
            m_passwordHintLabel->setText(QStringLiteral("格式：数字/中英文/下划线，至少6个字符"));
            m_passwordHintLabel->setStyleSheet("QLabel { color: #8a8a8a; font-size: 12px; }");
        }
        else if (passwordValid)
        {
            m_passwordHintLabel->setText(QStringLiteral("密码格式正确"));
            m_passwordHintLabel->setStyleSheet("QLabel { color: #0a8f46; font-size: 12px; }");
        }
        else
        {
            m_passwordHintLabel->setText(QStringLiteral("密码格式错误：仅支持数字/中英文/下划线，且至少6个字符"));
            m_passwordHintLabel->setStyleSheet("QLabel { color: #d93025; font-size: 12px; }");
        }

        m_registerButton->setEnabled(!m_isSubmitting && nicknameValid && phoneValid && passwordValid);
    }

    void RegisterWidget::setSubmitting(bool submitting)
    {
        m_isSubmitting = submitting;

        m_nicknameEdit->setEnabled(!submitting);
        m_phoneEdit->setEnabled(!submitting);
        m_passwordEdit->setEnabled(!submitting);
        m_goToLoginLink->setEnabled(!submitting);

        if (submitting)
        {
            m_loadingFrameIndex = 0;
            m_registerLoadingTimer->start(120);
            tickRegisterLoading();
        }
        else
        {
            m_registerLoadingTimer->stop();
            m_registerButton->setText(QStringLiteral("注册"));
            updateValidationState();
        }
    }

    void RegisterWidget::tickRegisterLoading()
    {
        static const char* frames[] = {"|", "/", "-", "\\"};
        const QString frame = QString::fromLatin1(frames[m_loadingFrameIndex % 4]);
        ++m_loadingFrameIndex;
        m_registerButton->setText(QStringLiteral("注册中 ") + frame);
        m_registerButton->setEnabled(false);
    }

    void RegisterWidget::chooseAvatar()
    {
        if (m_isSubmitting)
        {
            return;
        }

        const QString filePath = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("选择头像"),
            QString(),
            QStringLiteral("图片文件 (*.png *.jpg *.jpeg *.bmp)"));

        if (filePath.isEmpty())
        {
            return;
        }

        QPixmap avatar(filePath);
        if (avatar.isNull())
        {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("头像加载失败，请重新选择。"));
            return;
        }

        m_avatarFilePath = filePath;
        m_avatarPreview->setPixmap(avatar.scaled(m_avatarPreview->size(), Qt::KeepAspectRatioByExpanding,
                                                 Qt::SmoothTransformation));
    }

    void RegisterWidget::submitRegister()
    {
        if (m_isSubmitting)
        {
            return;
        }

        const QString phone = m_phoneEdit->text().trimmed();
        const QString password = m_passwordEdit->text();
        const QString nickname = m_nicknameEdit->text().trimmed();

        if (!isNicknameValid(nickname))
        {
            QMessageBox::warning(this, QStringLiteral("校验失败"), QStringLiteral("昵称格式不正确。"));
            return;
        }

        if (!isPhoneValid(phone))
        {
            QMessageBox::warning(this, QStringLiteral("校验失败"), QStringLiteral("手机号必须是11位数字。"));
            return;
        }

        if (!isPasswordValid(password))
        {
            QMessageBox::warning(this, QStringLiteral("校验失败"), QStringLiteral("密码格式不正确。"));
            return;
        }

        if (m_avatarFilePath.isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("校验失败"), QStringLiteral("请点击头像框上传头像。"));
            return;
        }

        QFile avatarFile(m_avatarFilePath);
        if (!avatarFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("头像读取失败，请重新选择。"));
            return;
        }

        contract::user::RegisterRequest request{};
        request.nickname = nickname;
        request.phone = phone;
        request.password = password;
        request.avatar = avatarFile.readAll();

        auto* appService = m_userApplicationService;
        if (appService == nullptr)
        {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("注册服务不可用，请稍后重试。"));
            return;
        }

        setSubmitting(true);

        QtConcurrent::run([appService, request]()
        {
            return appService->registerUser(request);
        }).then(this, [this](const contract::user::RegisterResponse& response)
        {
            setSubmitting(false);
            if (!response.success)
            {
                QMessageBox::warning(this, QStringLiteral("注册失败"), response.errMsg.value_or(QStringLiteral("未知错误")));
                return;
            }
            showUserIdDisplay(response.userId.value());
        });
    }

    void RegisterWidget::showUserIdDisplay(const QString& userId)
    {
        auto* userIdDisplay = new UserIdDisplayWidget(userId, nullptr);
        userIdDisplay->setAttribute(Qt::WA_DeleteOnClose, true);
        userIdDisplay->show();
        hide();
    }

    QString RegisterWidget::generateMockUserId()
    {
        QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        id.remove('-');
        return QStringLiteral("U") + id.left(12).toUpper();
    }

    void RegisterWidget::goToLogin()
    {
        if (m_isSubmitting)
        {
            return;
        }

        emit ui::common::UIEventBus::instance()->goToLoginRequested();
    }
}
