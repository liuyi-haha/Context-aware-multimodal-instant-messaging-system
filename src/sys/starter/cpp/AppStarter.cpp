#include "../include/AppStarter.h"

#include "sys/auth-context/starter/include/AuthContextStarter.h"
#include "sys/file-context/starter/include/FileContextStarter.h"
#include "sys/notification-context/starter/include/NotificationContextStarter.h"
#include "sys/relation-context/starter/include/RelationContextStarter.h"
#include "sys/user-context/starter/include/UserContextStarter.h"

namespace sys::starter
{
    void AppStarter::bootstrap()
    {
        static bool bootstrapped = false;
        if (bootstrapped)
        {
            return;
        }

        // Keep initialization sequence explicit and centralized.
        file::starter::FileContextStarter::bootstrap();
        user::starter::UserContextStarter::bootstrap();
        auth::starter::AuthContextStarter::bootstrap();
        notification::starter::NotificationContextStarter::bootstrap();
        relation::starter::RelationContextStarter::bootstrap();


        bootstrapped = true;
    }
}
