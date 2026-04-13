#pragma once
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QtConcurrent/QtConcurrent>
#include "sys/user-context/application/service/include/UserApplicationService.h"
#include "ui/common-widgets/ToastWidget.h"
#include "ui/relation-widgets/include/FriendAddingWidget.h"
#include "contract/system-provider/user-context-provider/UserView.hpp"

namespace ui::user_widgets
{
    class SearchWindow : public QDialog
    {
        Q_OBJECT

    private:
        QWidget* m_titleBar;
        QLabel* m_titleLabel;
        QPushButton* m_closeBtn;

        QWidget* m_searchArea;
        QLineEdit* m_searchBox;
        QPushButton* m_searchBtn;

        QWidget* m_resultContainer; // 结果容器
        QVBoxLayout* m_resultLayout; // 结果区域的布局管理器
        FriendAddingWidget* m_friendAddingWidget;

        sys::user::application::UserApplicationService* userApplicationService = QInjection::Inject;

    public:
        explicit SearchWindow(QWidget* parent = nullptr)
            : QDialog(parent)
              , m_friendAddingWidget(nullptr)
        {
            setupUI();
            setupConnections();
            setModal(true);
        }

        void setSearchText(const QString& text)
        {
            m_searchBox->setText(text);
            onSearchClicked();
        }

    protected:
        void showEvent(QShowEvent* event) override
        {
            QDialog::showEvent(event);
            m_searchBox->setFocus();
        }

    private:
        void setupUI()
        {
            setWindowTitle("搜索用户");
            setFixedSize(500, 600);
            setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
            setAttribute(Qt::WA_TranslucentBackground);

            QVBoxLayout* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(10, 10, 10, 10);
            mainLayout->setSpacing(0);

            // 设置圆角背景
            QWidget* centralWidget = new QWidget();
            centralWidget->setStyleSheet(R"(
                QWidget {
                    background-color: white;
                    border-radius: 8px;
                }
            )");
            QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
            centralLayout->setContentsMargins(0, 0, 0, 0);
            centralLayout->setSpacing(0);

            // 标题栏
            createTitleBar();
            centralLayout->addWidget(m_titleBar);

            // 搜索区域（固定高度）
            createSearchArea();
            centralLayout->addWidget(m_searchArea);

            // 结果区域（占据剩余空间）
            createResultArea();
            centralLayout->addWidget(m_resultContainer, 1); // stretch factor = 1

            mainLayout->addWidget(centralWidget);
        }

        void createTitleBar()
        {
            m_titleBar = new QWidget();
            m_titleBar->setFixedHeight(50);
            m_titleBar->setStyleSheet(R"(
                QWidget {
                    background-color: rgb(247, 247, 247);
                    border-top-left-radius: 8px;
                    border-top-right-radius: 8px;
                }
            )");

            QHBoxLayout* titleLayout = new QHBoxLayout(m_titleBar);
            titleLayout->setContentsMargins(15, 0, 10, 0);

            m_titleLabel = new QLabel("搜索用户");
            m_titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

            m_closeBtn = new QPushButton("×");
            m_closeBtn->setFixedSize(30, 30);
            m_closeBtn->setStyleSheet(R"(
                QPushButton {
                    border: none;
                    font-size: 20px;
                    font-weight: bold;
                    color: #999;
                }
                QPushButton:hover {
                    color: #333;
                }
            )");

            titleLayout->addWidget(m_titleLabel);
            titleLayout->addStretch();
            titleLayout->addWidget(m_closeBtn);
        }

        void createSearchArea()
        {
            m_searchArea = new QWidget();
            m_searchArea->setFixedHeight(80);
            m_searchArea->setStyleSheet("background-color: white;");

            QHBoxLayout* searchLayout = new QHBoxLayout(m_searchArea);
            searchLayout->setContentsMargins(20, 20, 20, 20);
            searchLayout->setSpacing(10);

            m_searchBox = new QLineEdit();
            m_searchBox->setPlaceholderText("请输入用户ID或手机号");
            m_searchBox->setStyleSheet(R"(
                QLineEdit {
                    padding: 8px 12px;
                    border: 1px solid #ddd;
                    border-radius: 4px;
                    background: white;
                    font-size: 12px;
                }
                QLineEdit:focus {
                    border: 1px solid #ddd;
                    outline: none;
                }
            )");

            m_searchBtn = new QPushButton("搜索");
            m_searchBtn->setFixedSize(80, 36);
            m_searchBtn->setStyleSheet(R"(
                QPushButton {
                    background-color: #07c160;
                    border: none;
                    border-radius: 4px;
                    font-size: 12px;
                    color: white;
                    font-weight: bold;
                }
                QPushButton:hover {
                    background-color: #06ad56;
                }
                QPushButton:pressed {
                    background-color: #059648;
                }
                QPushButton:disabled {
                    background-color: #9adba8;
                    color: rgba(255, 255, 255, 150);
                }
            )");

            searchLayout->addWidget(m_searchBox, 1);
            searchLayout->addWidget(m_searchBtn);
        }

        void createResultArea()
        {
            m_resultContainer = new QWidget();
            m_resultContainer->setStyleSheet("background-color: white;");
            m_resultLayout = new QVBoxLayout(m_resultContainer);
            m_resultLayout->setContentsMargins(0, 0, 0, 0);
            m_resultLayout->setSpacing(0);
        }

        void clearResult()
        {
            // 移除并删除旧的 FriendAddingWidget
            if (m_friendAddingWidget)
            {
                m_resultLayout->removeWidget(m_friendAddingWidget);
                m_friendAddingWidget->deleteLater();
                m_friendAddingWidget = nullptr;
            }
        }

        void setupConnections()
        {
            connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::close);
            connect(m_searchBtn, &QPushButton::clicked, this, &SearchWindow::onSearchClicked);
            connect(m_searchBox, &QLineEdit::returnPressed, this, &SearchWindow::onSearchClicked);
        }

        void onSearchClicked()
        {
            QString keyword = m_searchBox->text().trimmed();
            if (keyword.isEmpty())
            {
                return;
            }

            // 清空之前的结果
            clearResult();

            // 按钮置灰，文字改为"搜索中"
            m_searchBtn->setEnabled(false);
            m_searchBtn->setText("搜索中");

            contract::user::SearchUserRequest request;
            request.keyword = keyword;
            QtConcurrent::run([this, request]()
            {
                const auto response = userApplicationService->searchUser(request);
                return response;
            }).then(this, [this](const contract::user::SearchUserResponse& response)
            {
                // 恢复按钮状态
                m_searchBtn->setEnabled(true);
                m_searchBtn->setText("搜索");

                if (!response.success)
                {
                    common_widgets::ToastWidget::showToast(this, response.errMsg.value_or(""));
                    return;
                }

                if (response.userView.has_value())
                {
                    renderResults(response.userView.value());
                }
                else
                {
                    common_widgets::ToastWidget::showToast(this, "该用户不在");
                }
            });
        }

        void renderResults(const contract::user::UserView& userView)
        {
            // 确保之前的 widget 已被清理
            clearResult();
            // 创建新的 FriendAddingWidget
            m_friendAddingWidget = new FriendAddingWidget(userView, this);

            // 添加到布局管理器中，占据所有剩余空间
            m_resultLayout->addWidget(m_friendAddingWidget);

            // 可选：设置 widget 的大小策略，使其能够填充整个空间
            m_friendAddingWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }
    };
} // namespace ui::user_widgets
