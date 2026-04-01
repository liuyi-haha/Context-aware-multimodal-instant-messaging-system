#pragma once

#include "sys/common/component/Database.h"
#include "sys/relation-context/port/repository/include/ParticipantRepository.h"

namespace sys::relation::adapter
{
    class ParticipantRepositoryAdapter final : public port::ParticipantRepository
    {
    public:
        explicit ParticipantRepositoryAdapter(
            common::component::IPrivateDatabase* privateDatabase = common::component::PrivateDatabase::instance());

        void saveAll(const QVector<QSharedPointer<domain::Participant>>& participants) override;
        QSharedPointer<domain::Participant> of(const QString& participantId) override;
        QSharedPointer<domain::Participant> of(const QString& privateChatSessionId, const QString& userId) override;
        QList<QSharedPointer<domain::Participant>> ofChatSessionId(const QString& chatSessionId) override;
        QList<QSharedPointer<domain::Participant>> ofAll(const QString& userId, domain::ParticipantRole role) override;

    private:
        common::component::IPrivateDatabase* privateDatabase = nullptr;
    };
}
