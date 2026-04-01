//
// Created by 86150 on 2026/4/1.
//
#include "dependencyinjector.h"
#include "sys/message-context/port/client/include/RelationClient.h"
#pragma once
namespace sys::message::domain
{
    class SenderValidator
    {
    public:
        void checkSenderHasPermissionToSendMessage();

    private:
        port::RelationClient* relationClient = QInjection::Inject;
    };
}
