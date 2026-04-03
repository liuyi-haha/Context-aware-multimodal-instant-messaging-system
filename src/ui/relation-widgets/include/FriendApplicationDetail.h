//
// Created by 86150 on 2026/3/27.
//

#pragma once
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"
#include "dependencyinjector.h"
#include "sys/relation-context/application/service/include/RelationApplicationService.h"

namespace ui::relation_widgets
{
    class FriendApplicationDetail : public QWidget
    {
        Q_OBJECT

    public:
        explicit FriendApplicationDetail(
            QWidget* parent = nullptr,
            sys::relation::application::RelationApplicationService* relationApplicationService = QInjection::Inject);

        void setFriendApplicationView(const contract::relation::FriendApplicationView& friendApplicationView);

    protected:
        void resizeEvent(QResizeEvent* event) override;
        void paintEvent(QPaintEvent* event) override; // 添加paintEvent确保背景色生效

    private:
        static void applyStatusStyle(QLabel* statusLabel, contract::relation::FriendApplicationView::Status status);

        void createWidgets();
        void initStyle();
        void createLayout();
        void setupConnections();
        void centerCardWidget();
        void acceptCurrentApplicationAsync();
        void rejectCurrentApplicationAsync();

    private:
        QWidget* cardWidget = nullptr;
        QLabel* avatarLabel = nullptr;
        QLabel* nameLabel = nullptr;
        QLabel* userIdLabel = nullptr;
        QLabel* statusLabel = nullptr;

        QLabel* verificationTitleLabel = nullptr; // "验证消息"
        QLabel* verificationContentLabel = nullptr; // 验证消息内容
        QLabel* timeTitleLabel = nullptr; // "申请时间"
        QLabel* timeContentLabel = nullptr; // 申请时间内容

        QPushButton* acceptButton = nullptr;
        QPushButton* rejectButton = nullptr;

        contract::relation::FriendApplicationView currentView;
        sys::relation::application::RelationApplicationService* relationApplicationService = nullptr;
    };
}
