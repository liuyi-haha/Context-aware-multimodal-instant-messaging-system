//
// Created by 86150 on 2026/4/1.
//
#include "dependencyinjector.h"
#include "sys/message-context/port/client/include/RelationClient.h"
#include <QObject>
#pragma once
namespace sys::message::domain
{
    class SenderValidator : public QObject
    {
        Q_OBJECT

    public:
        void checkSenderHasPermissionToSendMessage();

        explicit SenderValidator(port::RelationClient* relationClient)
            : relationClient(relationClient)
        {
        }

    private:
        port::RelationClient* relationClient = QInjection::Inject;
    };
}
