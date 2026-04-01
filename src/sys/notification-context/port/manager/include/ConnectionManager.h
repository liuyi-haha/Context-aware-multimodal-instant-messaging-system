#pragma once
#include <QObject>
namespace sys::notification::port
{
    enum class ConnectionStatus
    {
        Connected,
        Disconnected,
        Connecting,
    };

    class ConnectionManager : public QObject
    {
        Q_OBJECT
    public:
        virtual ~ConnectionManager() = default;
        virtual void tryToConnect() = 0;
        virtual ConnectionStatus getConnectStatus() const = 0;
        virtual void setConnectionStatus(ConnectionStatus status) = 0;
    };
} // namespace sys::notification::port