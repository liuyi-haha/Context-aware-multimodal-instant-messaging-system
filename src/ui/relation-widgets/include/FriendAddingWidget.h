//
// Created by 86150 on 2026/4/3.
//


#pragma once
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainterPath>
#include <QPixmap>
#include "contract/system-provider/user-context-provider/UserView.hpp"
#include "sys/file-context/application/service/include/FileApplicationService.h"
#include <QPixmap>

#include "AddFriendRequestWidget.h"
#include "ui/common/UIEVentBus.h"

// todo @liuyi
namespace ui::user_widgets
{
    class FriendAddingWidget : public QWidget
    {
        Q_OBJECT

    public:
        FileApplicationService* fileApplicationService = QInjection::Inject;

        explicit FriendAddingWidget(const contract::user::UserView& userView, QWidget* parent = nullptr)
            : QWidget(parent),
              m_userView(userView)
        {
            createWidgets();
            initStyle();
            createLayout();
            setupConnections();
            updateUserInfo();

            // 异步加载头像
            QtConcurrent::run([this]()
            {
                return fileApplicationService->getFile(m_userView.avatarFileId);
            }).then(this, [this](const QByteArray& avatarData)
            {
                m_avatarLabel->setPixmap(roundAvatar(avatarData, m_avatarLabel->size()));
            });
        }

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

    protected:
        void resizeEvent(QResizeEvent* event) override
        {
            QWidget::resizeEvent(event);
            centerCardWidget();
        }

    private:
        void createWidgets()
        {
            // 卡片容器
            m_cardWidget = new QWidget(this);
            m_cardWidget->setObjectName("CardWidget");

            // 头像
            m_avatarLabel = new QLabel();
            m_avatarLabel->setFixedSize(80, 80);
            m_avatarLabel->setScaledContents(true);

            // 昵称
            m_nameLabel = new QLabel();
            m_nameLabel->setObjectName("NameLabel");

            // 用户ID
            m_userIdLabel = new QLabel();
            m_userIdLabel->setObjectName("UserIdLabel");

            // 申请好友按钮
            m_addFriendBtn = new QPushButton("申请好友");
            m_addFriendBtn->setObjectName("AddFriendButton");
        }

        void initStyle()
        {
            setStyleSheet(R"(
                FriendAddingWidget {
                    background-color: rgba(0, 0, 0, 0.5);
                }

                #CardWidget {
                    background-color: white;
                    border-radius: 12px;
                    border: 1px solid #e5e5e5;
                }

                #NameLabel {
                    font-size: 18px;
                    font-weight: bold;
                    color: #333;
                    margin-top: 10px;
                }

                #UserIdLabel {
                    font-size: 12px;
                    color: #999;
                    margin-top: 5px;
                }

                #AddFriendButton {
                    background-color: #07c160;
                    border: none;
                    border-radius: 6px;
                    font-size: 14px;
                    font-weight: bold;
                    color: white;
                    padding: 8px 20px;
                    min-width: 120px;
                }
                #AddFriendButton:hover {
                    background-color: #06ad56;
                }
                #AddFriendButton:pressed {
                    background-color: #059648;
                }
            )");
        }

        void createLayout()
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            mainLayout->addWidget(m_cardWidget, 0, Qt::AlignCenter);

            // 卡片内布局
            QVBoxLayout* cardLayout = new QVBoxLayout(m_cardWidget);
            cardLayout->setContentsMargins(40, 30, 40, 30);
            cardLayout->setSpacing(10);

            // 头像居中
            QHBoxLayout* avatarLayout = new QHBoxLayout();
            avatarLayout->addStretch();
            avatarLayout->addWidget(m_avatarLabel);
            avatarLayout->addStretch();
            cardLayout->addLayout(avatarLayout);

            cardLayout->addWidget(m_nameLabel, 0, Qt::AlignCenter);
            cardLayout->addWidget(m_userIdLabel, 0, Qt::AlignCenter);

            cardLayout->addSpacing(20);

            // 按钮居中
            QHBoxLayout* buttonLayout = new QHBoxLayout();
            buttonLayout->addStretch();
            buttonLayout->addWidget(m_addFriendBtn);
            buttonLayout->addStretch();
            cardLayout->addLayout(buttonLayout);
        }

        void setupConnections()
        {
            connect(m_addFriendBtn, &QPushButton::clicked, this, [this]()
            {
                auto addFriendRequestWidget = new relation_widgets::AddFriendRequestWidget(
                    m_nameLabel->text(), m_userIdLabel->text(), this);
                addFriendRequestWidget->setAttribute(Qt::WA_DeleteOnClose);
                addFriendRequestWidget->show();
            });
        }

        void updateUserInfo()
        {
            // 设置昵称
            m_nameLabel->setText(m_userView.nickname);
            // 设置用户ID
            m_userIdLabel->setText(m_userView.userId);

            // 设置头像
            if (m_userView.avatar.has_value())
            {
                QPixmap pixmap;
                if (pixmap.loadFromData(m_userView.avatar.value()))
                {
                    m_avatarLabel->setPixmap(pixmap);
                }
            }
            else
            {
                // TODO: 设置默认头像
                // m_avatarLabel->setPixmap(QPixmap(":/icons/default_avatar.png"));
            }
        }

        void centerCardWidget()
        {
            if (m_cardWidget)
            {
                int x = (width() - m_cardWidget->width()) / 2;
                int y = (height() - m_cardWidget->height()) / 2;
                m_cardWidget->move(x, y);
            }
        }

    private:
        contract::user::UserView m_userView;

        QWidget* m_cardWidget = nullptr;
        QLabel* m_avatarLabel = nullptr;
        QLabel* m_nameLabel = nullptr;
        QLabel* m_userIdLabel = nullptr;
        QPushButton* m_addFriendBtn = nullptr;
    };
}
