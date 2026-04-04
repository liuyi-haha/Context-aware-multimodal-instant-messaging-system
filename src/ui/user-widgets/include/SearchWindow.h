//
// Created by 86150 on 2026/4/3.
//

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

        QListWidget* m_resultList;

        QFutureWatcher<void>* m_searchWatcher;

    public:
        explicit SearchWindow(QWidget* parent = nullptr)
            : QDialog(parent),
              m_searchWatcher(nullptr)
        {
            setupUI();
            setupConnections();
            setModal(true); // 模态对话框，不关闭前不能操作其他窗口
        }

        ~SearchWindow()
        {
            if (m_searchWatcher)
            {
                m_searchWatcher->cancel();
                delete m_searchWatcher;
            }
        }

        void setSearchText(const QString& text)
        {
            m_searchBox->setText(text);
            onSearchClicked(); // 自动触发搜索
        }

    protected:
        void showEvent(QShowEvent* event) override
        {
            QDialog::showEvent(event);
            m_searchBox->setFocus(); // 显示时自动聚焦到搜索框
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

            // 搜索区域
            createSearchArea();
            centralLayout->addWidget(m_searchArea);

            // 结果列表
            createResultList();
            centralLayout->addWidget(m_resultList);

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

        void createResultList()
        {
            m_resultList = new QListWidget();
            m_resultList->setStyleSheet(R"(
                QListWidget {
                    border: none;
                    outline: none;
                    background-color: white;
                    border-bottom-left-radius: 8px;
                    border-bottom-right-radius: 8px;
                }
                QListWidget::item {
                    padding: 10px;
                    border-bottom: 1px solid #f0f0f0;
                }
                QListWidget::item:hover {
                    background-color: rgb(247, 247, 247);
                }
            )");
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

            // 按钮置灰，文字改为"搜索中"
            m_searchBtn->setEnabled(false);
            m_searchBtn->setText("搜索中");

            // 清空之前的结果
            m_resultList->clear();

            // 异步搜索
            QFuture<void> future = QtConcurrent::run([this, keyword]()
            {
                // TODO: 调用搜索接口
                // return searchUsers(keyword);
            });

            m_searchWatcher = new QFutureWatcher<void>();
            connect(m_searchWatcher, &QFutureWatcher<void>::finished, this, [this]()
            {
                // 恢复按钮
                m_searchBtn->setEnabled(true);
                m_searchBtn->setText("搜索");

                // TODO: 渲染数据
                // renderResults();
                m_searchWatcher->deleteLater();
                m_searchWatcher = nullptr;
            });

            m_searchWatcher->setFuture(future);
        }

        // TODO: 渲染搜索结果
        void renderResults()
        {
            // 根据搜索结果填充 m_resultList
        }
    };
} // namespace ui::search_widgets
