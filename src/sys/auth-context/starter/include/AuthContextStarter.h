#pragma once
namespace sys::auth::application
{
    class AuthApplicationService;
}

namespace sys::auth::starter
{
    class AuthContextStarter
    {
    public:
        static void bootstrap();
    };
}