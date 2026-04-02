//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <QObject>

namespace sys::message::port
{
    class RelationClient : public QObject
    {
        Q_OBJECT

    public:
        virtual void checkSenderHasPermissionToSendMessage(const QString& chatSessionId) = 0; // 如果没有权限则抛异常
    };
}
