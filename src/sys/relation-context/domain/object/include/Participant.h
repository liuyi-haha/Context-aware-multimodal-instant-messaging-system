//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QSharedPointer>
#include <QString>

#include "sys/common/RecipientRemark.hpp"

namespace sys::relation::domain
{
    enum class ParticipantRole
    {
        GroupOwner,
        GroupMember,
        PrivateMember
    };

    class Participant
    {
    public:
        static QSharedPointer<Participant> of(const QString& participantId,
                                              const QString& chatSessionId,
                                              const QString& userId,
                                              ParticipantRole role,
                                              const QString& remark);

        static QSharedPointer<Participant> of(const QString& participantId,
                                              const QString& chatSessionId,
                                              const QString& userId,
                                              ParticipantRole role);

        const QString& id() const;
        const QString& sessionId() const;
        const QString& userIdValue() const;
        ParticipantRole roleValue() const;
        QString remarkValue() const;
        bool hasRemark() const;

    private:
        Participant(const QString& participantId,
                    const QString& chatSessionId,
                    const QString& userId,
                    ParticipantRole role,
                    const QString& remark);
        Participant(const QString& participantId, const QString& chatSessionId, const QString& userId,
                    ParticipantRole role);

    private:
        QString participantId;
        QString chatSessionId;
        QString userId;
        ParticipantRole role;
        // 群聊成员的备注只能自己修改，单聊成员的备注只能有对方更改。群聊成员的备注可以被看见，好友给自己的备注不能被看见。
        std::optional<sys::core::RecipientRemark> remark;
    };
}
