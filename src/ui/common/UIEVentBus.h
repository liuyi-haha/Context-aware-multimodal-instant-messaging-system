//
// Created by 86150 on 2026/3/23.
//
#pragma once

#include <QObject>

#include "contract/system-provider/relation-context-provider/ChatSessionView.h"
#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"

namespace ui::common
{
    class UIEventBus final : public QObject
    {
        Q_OBJECT

    public:
        static UIEventBus* instance();

    signals:
        void navSelected(int index);
        void friendApplicationSelected(const contract::relation::FriendApplicationView& friendApplicationView);
        void chatSessionSelected(const contract::relation::ChatSessionView& chatSessionView);
        void friendApplicationStatusChanged(const QString& applicationId,
                                            contract::relation::FriendApplicationView::Status status);
        void goToLoginRequested();
        void goToRegisterRequested();

    private:
        explicit UIEventBus(QObject* parent = nullptr);
    };
}
