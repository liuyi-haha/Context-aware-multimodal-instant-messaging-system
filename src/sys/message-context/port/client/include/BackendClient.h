//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <QObject>

namespace sys::message::port
{
    class BackendClient : public QObject
    {
        Q_OBJECT

    public:
        virtual void sendTextMessage() = 0;
    };
}
