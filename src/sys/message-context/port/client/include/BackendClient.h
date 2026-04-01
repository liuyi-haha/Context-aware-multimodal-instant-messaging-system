//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <QObject>

#include "sys/message-context/domain/object/include/Message.h"

namespace sys::message::port
{
    class BackendClient : public QObject
    {
        Q_OBJECT

    public:
        virtual void sendTextMessage(const QString& chatSessionId, const QString& text) = 0;
    };
}
