//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include <QHBoxLayout>
#include <QLabel>
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
            QHBoxLayout* layout = new QHBoxLayout(this);
            layout->setContentsMargins(10, 5, 10, 5);

            m_titleLabel = new QLabel(m_chatSessionName, this);
            m_titleLabel->setStyleSheet(
                "QLabel {"
                "   font-size: 18px;"
                "   font-weight: bold;"
                "   color: #333333;"
                "}"
            );

            layout->addWidget(m_titleLabel);
            layout->addStretch();
            setStyleSheet("ChatSessionTitleWidget { background-color: #F5F5F5; border-bottom: 1px solid #E0E0E0; }");
            setFixedHeight(50);
        }

        void setChatName(const QString& name)
        {
            m_titleLabel->setText(name);
        }

    private:
        QLabel* m_titleLabel;
        QString m_chatSessionName;
    };
}
