//
// Created by 86150 on 2026/4/2.
//

#pragma once
#include "contract/system-provider/message-context-provider/MessageView.h"
#include "sys/message-context/domain/object/include/Message.h"
#include <QObject>

#include "dependencyinjector.h"
#include "sys/common/port/UserClient.h"

namespace sys::message::application
{
    class MessageViewAssembler : public QObject
    {
        Q_OBJECT

    public:
        explicit MessageViewAssembler(common::port::UserClient* userClient)
            : userClient(userClient)
        {
        }

        MessageViewAssembler() = default;

        contract::message::MessageView assemble(const QSharedPointer<domain::Message>& message);
        QList<contract::message::MessageView> assembleMany(const QList<QSharedPointer<domain::Message>>& messages);
        contract::message::MessageView assembleMyMessage(const QSharedPointer<domain::Message>& message);
        contract::message::MessageView assembleOthersMessage(const QSharedPointer<domain::Message>& message);

    private:
        common::port::UserClient* userClient = QInjection::Inject;
    };
}
