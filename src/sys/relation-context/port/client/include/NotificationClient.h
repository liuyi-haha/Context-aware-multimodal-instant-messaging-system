//
// Created by 86150 on 2026/3/30.
//

#pragma once
#include <QString>
#include <QObject>

namespace sys::relation::port
{
    class NotificationClient : public QObject
    {
        Q_OBJECT

    public:
        virtual int getUnreadChatSessionItemCount(const QString& chatSessionId) = 0;
    };
}
