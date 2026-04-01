//
// Created by 86150 on 2026/3/29.
//

#pragma once

#include <QObject>

#include "sys/notification-context/application/event/include/ConnectionStatusChanged.h"

namespace common
{
    class EventBus final : public QObject
    {
        Q_OBJECT

    public:
        static EventBus* instance()
        {
            static EventBus bus;
            return &bus;
        }

    signals:
        void connectionStatusChanged(const sys::notification::application::ConnectionStatusChanged& event);

    private:
        EventBus() = default;
    };
}
