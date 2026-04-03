//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <Qstyle>

#include "ui/auth-widgets/include/LoginWidget.h"

namespace ui::message_widgets
{
    class ChatSessionTitleWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit ChatSessionTitleWidget(const QString& chatSessionName, QWidget* parent = nullptr)
            : m_chatSessionName(chatSessionName), QWidget(parent)
        {
            m_titleLabel = new QLabel(m_chatSessionName, this);
            initStyle();
            initLayout();
        }

        void setChatName(const QString& name)
        {
            m_titleLabel->setText(name);
        }

    private:
        void initStyle()
        {
            setObjectName("titleWidget");
            qDebug() << "objectName:" << objectName(); // 打印验证
            setStyleSheet(
                "#titleWidget {"
                "   border-bottom: 1px solid #d0d0d0; "
                "   background-color: rgb(237, 237, 237);"
                "   border: 2px solid red;"
                "}"
                "QLabel {"
                "   font-size: 18px;"
                "   font-weight: 600;"
                "   background-color: rgb(237, 237, 237);"
                "}");
            setFixedHeight(70);
            QPalette pal;
            pal.setColor(QPalette::Window, QColor(237, 237, 237));
            this->setPalette(pal);
            this->setAutoFillBackground(true);
        }

        // void paintEvent(QPaintEvent* event) override
        // {
        //     QPainter painter(this);
        //     painter.fillRect(rect(), Qt::gray); // 填充灰色背景
        //     painter.setPen(QPen(Qt::red, 2));
        //     painter.drawRect(rect().adjusted(0, 0, -1, -1)); // 画红框
        //     QWidget::paintEvent(event); // 让子控件正常绘制
        // }

        void initLayout()
        {
            QHBoxLayout* layout = new QHBoxLayout(this);
            layout->setContentsMargins(20, 5, 10, 5);
            layout->addWidget(m_titleLabel);
            layout->addStretch();
        }

        QLabel* m_titleLabel;
        QString m_chatSessionName;
    };
}
