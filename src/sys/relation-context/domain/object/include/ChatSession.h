//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QString>

namespace sys::relation::domain
{
    enum class ChatSessionType
    {
        PrivateChatSessionType,
        GroupChatSessionType
    };

    class ChatSession
    {
    public:
        virtual ~ChatSession() = default;
        virtual ChatSessionType getType() const = 0;
    };
}