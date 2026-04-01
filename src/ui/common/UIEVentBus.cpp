#include "ui/common/UIEVentBus.h"

namespace ui::common
{
    UIEventBus::UIEventBus(QObject* parent)
        : QObject(parent)
    {
    }

    UIEventBus* UIEventBus::instance()
    {
        static UIEventBus bus;
        return &bus;
    }
}

