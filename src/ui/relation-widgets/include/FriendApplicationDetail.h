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

    private:
        static void applyStatusStyle(QLabel* statusLabel, contract::relation::FriendApplicationView::Status status);

        void initStyle();
        void createWidgets();
        void createLayout();
        void setupConnections();
        void acceptCurrentApplicationAsync();
        void rejectCurrentApplicationAsync();

    private:
        QLabel* avatarLabel = nullptr;
        QLabel* nameLabel = nullptr;
        QLabel* userIdLabel = nullptr;
        QLabel* statusLabel = nullptr;
        QLabel* verificationLabel = nullptr;
        QLabel* sendTimeLabel = nullptr;
        QPushButton* acceptButton = nullptr;
        QPushButton* rejectButton = nullptr;

        contract::relation::FriendApplicationView currentView;
        sys::relation::application::RelationApplicationService* relationApplicationService = nullptr;
    };
}
