//
// Created by 86150 on 2026/3/27.
//

#include "../include/FriendApplicationDetail.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "contract/system-provider/relation-context-provider/AcceptFriendApplication.h"
#include "contract/system-provider/relation-context-provider/RejectFriendApplication.h"
#include "ui/common/UIEVentBus.h"

namespace
{
    QPixmap roundAvatar(const QByteArray& avatarBytes, const QSize& size)
    {
        QPixmap source(size);
        source.fill(QColor(220, 220, 220));

        if (!avatarBytes.isEmpty())
        {
            QPixmap loaded;
            if (loaded.loadFromData(avatarBytes))
            {
                source = loaded.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
        }

        QPixmap rounded(size);
        rounded.fill(Qt::transparent);

        QPainter painter(&rounded);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path;
        path.addRoundedRect(QRect(QPoint(0, 0), size), 8, 8);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, source);

        return rounded;
    }
}

namespace ui::relation_widgets
{
    FriendApplicationDetail::FriendApplicationDetail(
        QWidget* parent,
        sys::relation::application::RelationApplicationService* relationApplicationService)
        : QWidget(parent),
          relationApplicationService(relationApplicationService)
    {
        createWidgets();
        initStyle();
        createLayout();
        setupConnections();
    }

    void FriendApplicationDetail::initStyle()
    {
        setStyleSheet(
            "FriendApplicationDetail, FriendApplicationDetail > *, QWidget#cardWidget, QWidget#cardWidget > * { "
            "   background-color: rgb(237, 237, 237); "
            "}"
            "QWidget#cardWidget { border: none; }"
            "QLabel { color: #222222; font-family: Microsoft YaHei; }"
            "#nameLabel { font-size: 20px; font-weight: 600; color: #1f1f1f; }"
            "#userIdLabel, #timeContentLabel { font-size: 12px; color: #7a7a7a; }"
            "#verificationContentLabel { font-size: 14px; color: #4f4f4f; }"
            "#verificationTitleLabel, #timeTitleLabel { font-size: 14px; color: #8e8e93; }"
            "#statusLabel { font-size: 12px; background: #f3f3f3; border-radius: 14px; padding: 0 12px; }"
        );
        setAttribute(Qt::WA_StyledBackground, true);

        nameLabel->setObjectName("nameLabel");
        userIdLabel->setObjectName("userIdLabel");
        verificationTitleLabel->setObjectName("verificationTitleLabel");
        verificationContentLabel->setObjectName("verificationContentLabel");
        timeTitleLabel->setObjectName("timeTitleLabel");
        timeContentLabel->setObjectName("timeContentLabel");
        statusLabel->setObjectName("statusLabel");
        acceptButton->setObjectName("acceptButton");
        rejectButton->setObjectName("rejectButton");
        cardWidget->setObjectName("cardWidget");
    }

    void FriendApplicationDetail::paintEvent(QPaintEvent* event)
    {
        // 确保背景色被绘制
        QPainter painter(this);
        painter.fillRect(rect(), QColor(237, 237, 237));
        QWidget::paintEvent(event);
    }

    void FriendApplicationDetail::createWidgets()
    {
        cardWidget = new QWidget(this);
        cardWidget->setObjectName(QStringLiteral("cardWidget"));
        cardWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        cardWidget->setFixedWidth(500);

        avatarLabel = new QLabel(cardWidget);
        avatarLabel->setFixedSize(64, 64);
        avatarLabel->setAlignment(Qt::AlignCenter);

        nameLabel = new QLabel(cardWidget);
        nameLabel->setStyleSheet("font-size: 16px; font-weight: 500; color: #111111;");
        nameLabel->setAlignment(Qt::AlignLeft);

        userIdLabel = new QLabel(cardWidget);
        userIdLabel->setStyleSheet("font-size: 13px; color: #8e8e93;");
        userIdLabel->setAlignment(Qt::AlignLeft);

        statusLabel = new QLabel(cardWidget);
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setFixedHeight(28);
        statusLabel->setMinimumWidth(80);

        // 左边的标题标签（淡色）
        verificationTitleLabel = new QLabel(QStringLiteral("验证消息"), cardWidget);
        verificationTitleLabel->setStyleSheet("font-size: 14px; color: #8e8e93;");
        verificationTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        verificationTitleLabel->setFixedWidth(80);

        verificationContentLabel = new QLabel(cardWidget);
        verificationContentLabel->setWordWrap(true);
        verificationContentLabel->setAlignment(Qt::AlignLeft);
        verificationContentLabel->setStyleSheet("font-size: 14px; color: #3a3a3a; line-height: 1.5;");
        verificationContentLabel->setMinimumHeight(50);

        timeTitleLabel = new QLabel(QStringLiteral("申请时间"), cardWidget);
        timeTitleLabel->setStyleSheet("font-size: 14px; color: #8e8e93;");
        timeTitleLabel->setAlignment(Qt::AlignLeft);
        timeTitleLabel->setFixedWidth(80);

        timeContentLabel = new QLabel(cardWidget);
        timeContentLabel->setStyleSheet("font-size: 14px; color: #3a3a3a;");
        timeContentLabel->setAlignment(Qt::AlignLeft);

        acceptButton = new QPushButton(QStringLiteral("同意"), cardWidget);
        acceptButton->setFixedHeight(36);
        acceptButton->setMinimumWidth(100);
        acceptButton->setStyleSheet(
            "QPushButton { background-color: #07c160; color: white; border: none; border-radius: 18px; font-size: 14px; font-weight: 500; }"
            "QPushButton:hover { background-color: #06ad56; }"
            "QPushButton:pressed { background-color: #059a4a; }"
            "QPushButton:disabled { background-color: #c8e6d9; }"
        );

        rejectButton = new QPushButton(QStringLiteral("拒绝"), cardWidget);
        rejectButton->setFixedHeight(36);
        rejectButton->setMinimumWidth(100);
        rejectButton->setStyleSheet(
            "QPushButton { background-color: #f5f5f5; color: #111111; border: 1px solid #e5e5e5; border-radius: 18px; font-size: 14px; font-weight: 500; }"
            "QPushButton:hover { background-color: #ebebeb; border-color: #d0d0d0; }"
            "QPushButton:pressed { background-color: #e0e0e0; }"
            "QPushButton:disabled { background-color: #f5f5f5; color: #b0b0b0; }"
        );
    }

    void FriendApplicationDetail::createLayout()
    {
        // 顶部区域：头像 + 信息（名称和账号）
        auto* avatarLayout = new QVBoxLayout();
        avatarLayout->setContentsMargins(0, 0, 0, 0);
        avatarLayout->addWidget(avatarLabel);
        avatarLayout->addStretch();

        auto* infoLayout = new QVBoxLayout();
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(6);
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(userIdLabel);
        infoLayout->addStretch();

        auto* topLeftLayout = new QHBoxLayout();
        topLeftLayout->setContentsMargins(0, 0, 0, 0);
        topLeftLayout->setSpacing(16);
        topLeftLayout->addLayout(avatarLayout);
        topLeftLayout->addLayout(infoLayout, 1);
        topLeftLayout->addStretch();

        auto* topLayout = new QHBoxLayout();
        topLayout->setContentsMargins(0, 0, 0, 0);
        topLayout->addLayout(topLeftLayout);
        topLayout->addWidget(statusLabel, 0, Qt::AlignTop);

        // 分隔线
        QFrame* line = new QFrame(cardWidget);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("background-color: #d0d0d0;");
        line->setFixedHeight(1);

        // 验证消息行（左右布局）
        auto* verificationLayout = new QHBoxLayout();
        verificationLayout->setContentsMargins(0, 0, 0, 0);
        verificationLayout->setSpacing(16);
        verificationLayout->addWidget(verificationTitleLabel);
        verificationLayout->addWidget(verificationContentLabel, 1);

        // 申请时间行（左右布局）
        auto* timeLayout = new QHBoxLayout();
        timeLayout->setContentsMargins(0, 0, 0, 0);
        timeLayout->setSpacing(16);
        timeLayout->addWidget(timeTitleLabel);
        timeLayout->addWidget(timeContentLabel, 1);

        // 按钮区域
        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->setContentsMargins(0, 8, 0, 0);
        buttonLayout->setSpacing(12);
        buttonLayout->addStretch();
        buttonLayout->addWidget(acceptButton);
        buttonLayout->addWidget(rejectButton);
        buttonLayout->addStretch();

        // 卡片主布局
        auto* cardMainLayout = new QVBoxLayout(cardWidget);
        cardMainLayout->setContentsMargins(24, 24, 24, 24);
        cardMainLayout->setSpacing(16);
        cardMainLayout->addLayout(topLayout);
        cardMainLayout->addWidget(line);
        cardMainLayout->addLayout(verificationLayout);
        cardMainLayout->addLayout(timeLayout);
        cardMainLayout->addLayout(buttonLayout);
        cardMainLayout->addStretch();

        // 外层布局
        auto* outerLayout = new QVBoxLayout(this);
        outerLayout->setContentsMargins(0, 60, 0, 0);
        outerLayout->addWidget(cardWidget, 0, Qt::AlignHCenter);
        outerLayout->addStretch();
    }

    void FriendApplicationDetail::resizeEvent(QResizeEvent* event)
    {
        QWidget::resizeEvent(event);
        centerCardWidget();
    }

    void FriendApplicationDetail::centerCardWidget()
    {
        if (cardWidget)
        {
            int x = (width() - cardWidget->width()) / 2;
            cardWidget->move(x, cardWidget->y());
        }
    }

    void FriendApplicationDetail::setupConnections()
    {
        connect(acceptButton, &QPushButton::clicked, this, &FriendApplicationDetail::acceptCurrentApplicationAsync);
        connect(rejectButton, &QPushButton::clicked, this, &FriendApplicationDetail::rejectCurrentApplicationAsync);
    }

    void FriendApplicationDetail::setFriendApplicationView(
        const contract::relation::FriendApplicationView& friendApplicationView)
    {
        currentView = friendApplicationView;

        const QString displayName = friendApplicationView.name.trimmed().isEmpty()
                                        ? friendApplicationView.peerUserId
                                        : friendApplicationView.name;

        avatarLabel->setPixmap(roundAvatar(friendApplicationView.peerUserAvatar.value_or(QByteArray()),
                                           avatarLabel->size()));
        nameLabel->setText(displayName);
        userIdLabel->setText(QStringLiteral("账号: %1").arg(friendApplicationView.peerUserId));
        verificationContentLabel->setText(friendApplicationView.verificationMessage.isEmpty()
                                              ? QStringLiteral("(空白)")
                                              : friendApplicationView.verificationMessage);
        timeContentLabel->setText(friendApplicationView.applyTime.toString("yyyy-MM-dd HH:mm"));

        statusLabel->setText(contract::relation::FriendApplicationView::statusToText(friendApplicationView.status));
        applyStatusStyle(statusLabel, friendApplicationView.status);

        const bool showActions = friendApplicationView.status ==
            contract::relation::FriendApplicationView::Status::Pending;
        acceptButton->setVisible(showActions);
        rejectButton->setVisible(showActions);

        centerCardWidget();
    }

    void FriendApplicationDetail::applyStatusStyle(
        QLabel* statusLabel,
        const contract::relation::FriendApplicationView::Status status)
    {
        if (statusLabel == nullptr)
        {
            return;
        }

        QString style = "font-size: 12px; color: #8e8e93; background: #f5f5f5; border-radius: 14px; padding: 4px 16px;";
        if (status == contract::relation::FriendApplicationView::Status::Pending)
        {
            style = "font-size: 12px; color: #07c160; background: #e8f8ef; border-radius: 14px; padding: 4px 16px;";
        }
        else if (status == contract::relation::FriendApplicationView::Status::Accepted)
        {
            style = "font-size: 12px; color: #576b95; background: #f0f2f8; border-radius: 14px; padding: 4px 16px;";
        }
        else if (status == contract::relation::FriendApplicationView::Status::Rejected)
        {
            style = "font-size: 12px; color: #fa5151; background: #feeef0; border-radius: 14px; padding: 4px 16px;";
        }

        statusLabel->setStyleSheet(style);
    }

    void FriendApplicationDetail::acceptCurrentApplicationAsync()
    {
        if (relationApplicationService == nullptr || currentView.applicationId.trimmed().isEmpty())
        {
            return;
        }

        acceptButton->setEnabled(false);
        rejectButton->setEnabled(false);

        contract::relation::AcceptFriendApplicationRequest request;
        request.applicationId = currentView.applicationId;

        QtConcurrent::run([service = relationApplicationService, request]()
        {
            return service->acceptFriendApplication(request);
        }).then(this, [this](const contract::relation::AcceptFriendApplicationResponse& response)
        {
            acceptButton->setEnabled(true);
            rejectButton->setEnabled(true);

            if (!response.success)
            {
                QMessageBox::warning(this,
                                     QStringLiteral("操作失败"),
                                     response.errMsg.value_or(QStringLiteral("同意好友申请失败")));
                return;
            }

            currentView.status = contract::relation::FriendApplicationView::Status::Accepted;
            statusLabel->setText(contract::relation::FriendApplicationView::statusToText(currentView.status));
            applyStatusStyle(statusLabel, currentView.status);
            acceptButton->setVisible(false);
            rejectButton->setVisible(false);

            emit ui::common::UIEventBus::instance()->friendApplicationStatusChanged(
                currentView.applicationId,
                currentView.status);
        });
    }

    void FriendApplicationDetail::rejectCurrentApplicationAsync()
    {
        if (relationApplicationService == nullptr || currentView.applicationId.trimmed().isEmpty())
        {
            return;
        }

        acceptButton->setEnabled(false);
        rejectButton->setEnabled(false);

        contract::relation::RejectFriendApplicationRequest request;
        request.friendApplicationId = currentView.applicationId;

        QtConcurrent::run([service = relationApplicationService, request]()
        {
            return service->rejectFriendApplication(request);
        }).then(this, [this](const contract::relation::RejectFriendApplicationResponse& response)
        {
            acceptButton->setEnabled(true);
            rejectButton->setEnabled(true);

            if (!response.success)
            {
                QMessageBox::warning(this,
                                     QStringLiteral("操作失败"),
                                     response.errMsg.value_or(QStringLiteral("拒绝好友申请失败")));
                return;
            }

            currentView.status = contract::relation::FriendApplicationView::Status::Rejected;
            statusLabel->setText(contract::relation::FriendApplicationView::statusToText(currentView.status));
            applyStatusStyle(statusLabel, currentView.status);
            acceptButton->setVisible(false);
            rejectButton->setVisible(false);

            emit ui::common::UIEventBus::instance()->friendApplicationStatusChanged(
                currentView.applicationId,
                currentView.status);
        });
    }
}
