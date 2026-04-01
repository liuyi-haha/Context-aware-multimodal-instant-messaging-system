//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QString>

#include "sys/relation-context/domain/object/include/ChatSession.h"

namespace sys::relation::domain
{
    class GroupName
    {
    private:
        QString name;
    };

    class GroupChatSession : public ChatSession
    {
    public:
        ChatSessionType getType() const override;

    private:
        QString groupChatSessionId;
        GroupName name;
    };
}
