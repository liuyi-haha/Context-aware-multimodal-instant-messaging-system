//
// Created by 86150 on 2026/4/8.
//
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFormLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "sys/relation-context/application/service/include/RelationApplicationService.h"
#pragma once
namespace ui::relation_widgets
{
    class AddFriendRequestWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit AddFriendRequestWidget(const QString& nickname, const QString& toUserId, QWidget* parent = nullptr);
        void onSendClicked();

    private:
        QString m_toUserId;
        QLineEdit* m_remarkEdit;
        QTextEdit* m_messageEdit;
        QPushButton* sendButton;
        QPushButton* cancelButton;
        sys::relation::application::RelationApplicationService* relationApplicationService = QInjection::Inject;
    };
}
