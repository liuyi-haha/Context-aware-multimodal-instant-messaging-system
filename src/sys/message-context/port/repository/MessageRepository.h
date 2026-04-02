//
// Created by 86150 on 2026/4/2.
//

#pragma once
#include <QSharedPointer>
#include <QObject>

#include "sys/message-context/domain/object/include/Message.h"

namespace sys::message::port
{
    class MessageRepository : public QObject
    {
        Q_OBJECT

    public:
        virtual void save(const QSharedPointer<domain::Message>) = 0;
        virtual QSharedPointer<domain::Message> of(const QString& messageId) = 0;
    };
}
