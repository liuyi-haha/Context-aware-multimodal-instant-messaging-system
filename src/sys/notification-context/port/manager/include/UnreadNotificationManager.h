//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QHash>

namespace sys::notification::domain
{
    class UnreadNotificationManager
    {
    public:
        virtual int getFriendApplicationColumnCount() = 0;
        virtual int getMessageColumnCount() = 0;
        virtual int getchatSessionItemCounts(const QString& chatSessionId) = 0;
    };
}