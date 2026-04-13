#include "../include/ConnectionStatusChangedEventPublisherAdapter.h"

#include "common/EventBus.h"
#include "ui/common/UIEVentBus.h"

namespace sys::notification::adapter
{
    void ConnectionStatusChangedEventPublisherAdapter::publish(const application::ConnectionStatusChanged& event)
    {
        // todo @liuyi 这里
        ui::common::ConnectionStatus status = ui::common::ConnectionStatus::Disconnected;
        switch (event.status)
        {
        case port::ConnectionStatus::Connected:
            status = ui::common::ConnectionStatus::Connected;
            break;
        case port::ConnectionStatus::Connecting:
            status = ui::common::ConnectionStatus::Connecting;
            break;
        case port::ConnectionStatus::Disconnected:
            status = ui::common::ConnectionStatus::Disconnected;
            break;
        default:
            break;
        }
        // todo @liuyi 左侧窗口要订阅这个信号
        emit ui::common::UIEventBus::instance()->connectionStatusChanged(status);
    }
}
