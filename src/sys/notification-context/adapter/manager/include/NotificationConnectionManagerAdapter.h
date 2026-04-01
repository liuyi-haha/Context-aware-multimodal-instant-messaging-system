#pragma once

#include "sys/notification-context/port/manager/include/ConnectionManager.h"

namespace sys::notification::adapter
{
    class IWebsocketGateway
    {
    public:
        virtual ~IWebsocketGateway() = default;
        virtual void open() = 0;
    };

    class WebsocketGateway final : public IWebsocketGateway
    {
    public:
        void open() override;
    };

    class NotificationConnectionManager final : public port::ConnectionManager
    {
    public:
        explicit NotificationConnectionManager(IWebsocketGateway* websocketGateway = nullptr);

        void tryToConnect() override;
        port::ConnectionStatus getConnectStatus() const override;
        void setConnectionStatus(port::ConnectionStatus status) override;

    private:
        port::ConnectionStatus m_status = port::ConnectionStatus::Disconnected;
        WebsocketGateway defaultWebsocketGateway;
        IWebsocketGateway* websocketGateway = nullptr;
    };
}
