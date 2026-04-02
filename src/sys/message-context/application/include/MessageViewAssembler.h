//
// Created by 86150 on 2026/4/2.
//

#pragma once
#include "contract/system-provider/message-context-provider/MessageView.h"
#include "sys/message-context/domain/object/include/Message.h"
#include <QObject>

namespace sys::message::application
{
    class MessageViewAssembler : public QObject
    {
        Q_OBJECT

    public:
        contract::message::MessageView assemble(const QSharedPointer<domain::Message>& message);
    };
}
