#include "../include/BackendEventSubscriber.h"

#include "sys/common/component/Websocket.h"
#include "sys/notification-context/application/handler/include/BackendEventHandler.h"
#include "sys/common/component/Websocket.h"

namespace sys::notification::remote
{
    BackendEventSubscriber::BackendEventSubscriber(
        application::BackendEventHandler* backendEventHandler,
        QObject* parent)
        : backendEventHandler(backendEventHandler),
          QObject(parent)
    {
        connect(&Websocket::instance(), &Websocket::connected, this, &BackendEventSubscriber::subscribeConnectedEvent);
    }

    void BackendEventSubscriber::subscribeConnectedEvent()
    {
        backendEventHandler->handleConnectedEvent();
    }
}
