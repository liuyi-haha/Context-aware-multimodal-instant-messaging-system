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
        connect(&Websocket::instance(), &Websocket::disconnected, this,
                &BackendEventSubscriber::subscribeDisConnectedEvent);
        connect(&Websocket::instance(), &Websocket::connectFailed, this,
                &BackendEventSubscriber::subscribeConnectErrorEvent);
    }

    void BackendEventSubscriber::subscribeConnectedEvent()
    {
        backendEventHandler->handleConnectedEvent();
    }

    void BackendEventSubscriber::subscribeDisConnectedEvent()
    {
        backendEventHandler->handleDisconnectedEvent();
    }

    void BackendEventSubscriber::subscribeConnectErrorEvent(const QString& error)
    {
        qDebug() << "BackendEventSubscriber say: Websocket connection error: " << error;
        backendEventHandler->handleConnectFailedEvent();
    }
}
