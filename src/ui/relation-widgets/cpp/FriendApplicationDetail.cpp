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
        source.fill(QColor(242, 242, 242));

        QPixmap loaded;
        if (!avatarBytes.isEmpty() && loaded.loadFromData(avatarBytes))
        {
            source = loaded.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        }

        QPixmap rounded(size);
        rounded.fill(Qt::transparent);

        QPainter painter(&rounded);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QPainterPath path;
        path.addRoundedRect(QRect(QPoint(0, 0), size), 10, 10);
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
        initStyle();
        createWidgets();
        createLayout();
        setupConnections();
    }

    void FriendApplicationDetail::initStyle()
    {
        setObjectName(QStringLiteral("friendApplicationDetail"));
        setStyleSheet(
            "#friendApplicationDetail { background-color: white; }"
            "QLabel { color: #222222; font-family: Microsoft YaHei; }"
            "QPushButton { min-height: 34px; padding: 0 18px; border-radius: 6px; font-family: Microsoft YaHei; }"
        );
    }

    void FriendApplicationDetail::createWidgets()
    {
        avatarLabel = new QLabel(this);
        avatarLabel->setFixedSize(72, 72);

        nameLabel = new QLabel(this);
        nameLabel->setStyleSheet("font-size: 20px; font-weight: 600; color: #1f1f1f;");

        userIdLabel = new QLabel(this);
        userIdLabel->setStyleSheet("font-size: 12px; color: #7a7a7a;");

        statusLabel = new QLabel(this);
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setFixedHeight(28);

        verificationLabel = new QLabel(this);
        verificationLabel->setWordWrap(true);
        verificationLabel->setStyleSheet("font-size: 14px; color: #4f4f4f;");

        sendTimeLabel = new QLabel(this);
        sendTimeLabel->setStyleSheet("font-size: 12px; color: #8a8a8a;");

        acceptButton = new QPushButton(QStringLiteral("同意"), this);
        acceptButton->setStyleSheet("QPushButton { background-color: #05ab4b; color: white; border: none; }");

        rejectButton = new QPushButton(QStringLiteral("拒绝"), this);
        rejectButton->setStyleSheet(
            "QPushButton { background-color: #f3f3f3; color: #444444; border: 1px solid #e5e5e5; }");
    }

    void FriendApplicationDetail::createLayout()
    {
        auto* identityLayout = new QVBoxLayout();
        identityLayout->setContentsMargins(0, 0, 0, 0);
        identityLayout->setSpacing(4);
        identityLayout->addWidget(nameLabel);
        identityLayout->addWidget(userIdLabel);

        auto* headerLayout = new QHBoxLayout();
        headerLayout->setContentsMargins(0, 0, 0, 0);
        headerLayout->setSpacing(12);
        headerLayout->addWidget(avatarLabel, 0, Qt::AlignTop);
        headerLayout->addLayout(identityLayout, 1);
        headerLayout->addWidget(statusLabel, 0, Qt::AlignTop);

        auto* actionLayout = new QHBoxLayout();
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(10);
        actionLayout->addWidget(acceptButton);
        actionLayout->addWidget(rejectButton);
        actionLayout->addStretch();

        auto* rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(22, 24, 22, 24);
        rootLayout->setSpacing(14);
        rootLayout->addLayout(headerLayout);
        rootLayout->addSpacing(4);
        rootLayout->addWidget(new QLabel(QStringLiteral("验证消息"), this));
        rootLayout->addWidget(verificationLabel);
        rootLayout->addWidget(sendTimeLabel);
        rootLayout->addSpacing(8);
        rootLayout->addLayout(actionLayout);
        rootLayout->addStretch();
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
        userIdLabel->setText(QStringLiteral("用户ID: %1").arg(friendApplicationView.peerUserId));
        verificationLabel->setText(friendApplicationView.verificationMessage);
        sendTimeLabel->setText(
            QStringLiteral("申请时间: %1").arg(friendApplicationView.applyTime.toString("yyyy-MM-dd HH:mm:ss")));

        statusLabel->setText(contract::relation::FriendApplicationView::statusToText(friendApplicationView.status));
        applyStatusStyle(statusLabel, friendApplicationView.status);

        const bool showActions = friendApplicationView.status ==
            contract::relation::FriendApplicationView::Status::Pending;
        acceptButton->setVisible(showActions);
        rejectButton->setVisible(showActions);
    }

    void FriendApplicationDetail::applyStatusStyle(
        QLabel* statusLabel,
        const contract::relation::FriendApplicationView::Status status)
    {
        if (statusLabel == nullptr)
        {
            return;
        }

        QString style = "font-size: 12px; color: #707070; background: #f3f3f3; border-radius: 14px; padding: 0 12px;";
        if (status == contract::relation::FriendApplicationView::Status::Pending)
        {
            style = "font-size: 12px; color: #05ab4b; background: #e8f8ec; border-radius: 14px; padding: 0 12px;";
        }
        else if (status == contract::relation::FriendApplicationView::Status::Accepted)
        {
            style = "font-size: 12px; color: #267dff; background: #e8f1ff; border-radius: 14px; padding: 0 12px;";
        }
        else if (status == contract::relation::FriendApplicationView::Status::Rejected)
        {
            style = "font-size: 12px; color: #b25f5f; background: #fbefef; border-radius: 14px; padding: 0 12px;";
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
