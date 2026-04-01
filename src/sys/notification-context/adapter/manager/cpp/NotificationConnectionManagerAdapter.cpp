#include "../include/NotificationConnectionManagerAdapter.h"

#include "sys/common/component/Websocket.h"

namespace sys::notification::adapter
{
    void WebsocketGateway::open()
    {
        Websocket::instance().open();
    }

    NotificationConnectionManager::NotificationConnectionManager(
        IWebsocketGateway* websocketGateway)
        : websocketGateway(websocketGateway == nullptr
                               ? static_cast<IWebsocketGateway*>(&defaultWebsocketGateway)
                               : websocketGateway)
    {
    }

    void NotificationConnectionManager::tryToConnect()
    {
        if (m_status == port::ConnectionStatus::Connected || m_status == port::ConnectionStatus::Connecting)
        {
            return;
        }

        setConnectionStatus(port::ConnectionStatus::Connecting);
        websocketGateway->open();
    }

    port::ConnectionStatus NotificationConnectionManager::getConnectStatus() const
    {
        return m_status;
    }

    void NotificationConnectionManager::setConnectionStatus(const port::ConnectionStatus status)
    {
        m_status = status;
    }
}
