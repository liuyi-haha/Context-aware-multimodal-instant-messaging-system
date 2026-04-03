//
// Created by 86150 on 2026/3/31.
//

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QAbstractItemView>
#include <QGraphicsView>
#include "MessageListModel.h"
#include "MessageDelegate.h"
#include "sys/message-context/application/include/MessageApplicationService.h"

namespace ui::message_widgets
{
    class MessageListWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit MessageListWidget(const QString& chatSessionId, QWidget* parent = nullptr)
            : m_chatSessionId(chatSessionId),
              QWidget(parent)
        {
            // 创建布局
            QVBoxLayout* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);

            // 创建列表视图
            // listView->setSelectionMode(QAbstractItemView::SingleSelection);
            // listView->setMouseTracking(true);
            // listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            // listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            // listView->setUniformItemSizes(true);


            listView = new QListView(this);
            listView->setSelectionMode(QAbstractItemView::NoSelection);
            listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            listView->setSelectionBehavior(QAbstractItemView::SelectRows);
            listView->setMouseTracking(false);
            // 确保整个 item 都重绘
            listView->setStyleSheet(R"(
            QListView {
                border: none;
                outline: none;
            }
            QListView::item {
                padding: 0px;
            }
        )");

            // 创建模型和委托
            m_model = new MessageListModel(this);
            m_delegate = new MessageDelegate(this);

            listView->setModel(m_model);
            listView->setItemDelegate(m_delegate);

            layout->addWidget(listView);

            // 连接点击信号
            connect(listView, &QListView::clicked, this, &MessageListWidget::onItemClicked);

            // 异步加载消息
            QtConcurrent::run([this]()-> QList<contract::message::MessageView>
            {
                return messageApplicationService->getRecentMessages(m_chatSessionId, 20);
            }).then(this, [this](const QList<contract::message::MessageView>& views)
            {
                m_model->setViews(views);
            });
        }

        // 添加消息
        void addMessages(const QList<contract::message::MessageView>& msgs)
        {
            m_model->addMessages(msgs);
        }

        // 获取模型指针（用于异步加载）
        MessageListModel* model() const
        {
            return m_model;
        }

        // 滚动到底部
        void scrollToBottom()
        {
            if (m_model->rowCount() > 0)
            {
                listView->scrollTo(m_model->index(m_model->rowCount() - 1));
            }
        }

        // 滚动到指定消息
        void scrollToMessage(const QString& msgId)
        {
            for (int i = 0; i < m_model->rowCount(); ++i)
            {
                auto msg = m_model->getMessage(i);
                if (msg.msgId == msgId)
                {
                    listView->scrollTo(m_model->index(i));
                    break;
                }
            }
        }

    signals:
        // 图片点击信号
        void imageClicked(int msgId, const QString& fileId);

        // 文件下载信号
        void fileDownloadRequested(int msgId, const QString& fileId, const QString& fileName);

        // 文件打开信号（已下载的文件）
        void fileOpenRequested(int msgId, const QString& localPath);

        // 语音播放信号
        void audioPlayRequested(int msgId, const QString& fileId);

    private slots:
        void onItemClicked(const QModelIndex& index)
        {
            if (!index.isValid())
                return;

            auto msg = m_model->getMessage(index.row());

            switch (msg.messageType)
            {
            case contract::message::MessageView::MessageType::Image:
                {
                    // todo @liuyi
                    break;
                }
            case contract::message::MessageView::MessageType::File:
                {
                    // todo @liuyi
                    break;
                }
            case contract::message::MessageView::MessageType::Audio:
                {
                    // todo @liuyi
                }
            default:
                break;
            }
        }

    private:
        QListView* listView;
        MessageListModel* m_model;
        MessageDelegate* m_delegate;
        QString m_chatSessionId;
        sys::message::application::MessageApplicationService* messageApplicationService = QInjection::Inject;
    };
}
