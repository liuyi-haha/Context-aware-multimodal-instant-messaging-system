//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include "dependencypool.h"
#include "sys/message-context/application/include/MessageApplicationService.h"

namespace sys::message::starter
{
    static void start()
    {
        QInjection::addSingleton(new application::MessageApplicationService);
    }
};
