#pragma once
namespace sys::notification::application
{
    class NotificationApplicationService;
}

namespace sys::notification::starter
{
    class NotificationContextStarter
    {
    public:
        static void bootstrap();
    };
}