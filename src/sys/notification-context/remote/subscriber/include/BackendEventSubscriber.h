#pragma once
#include <QObject>

#include "dependencyinjector.h"
#include "sys/notification-context/application/handler/include/BackendEventHandler.h"

namespace sys::notification::remote
{
    class BackendEventSubscriber : public QObject
    {
        Q_OBJECT

    public:
        BackendEventSubscriber(
            application::BackendEventHandler* backendEventHandler = QInjection::Inject,
            QObject* parent = nullptr);

    public slots:
        void subscribeConnectedEvent();
        void subscribeDisConnectedEvent();
        void subscribeConnectErrorEvent(const QString& error);

    private:
        application::BackendEventHandler* backendEventHandler = nullptr;
    };
}
