#pragma once

#include "sys/common/component/Database.h"
#include "sys/relation-context/port/repository/include/PrivateChatSessionRepository.h"

namespace sys::relation::adapter
{
    class PrivateChatSessionRepositoryAdapter final : public port::PrivateChatSessionRepository
    {
    public:
        explicit PrivateChatSessionRepositoryAdapter(
            common::component::IPrivateDatabase* privateDatabase = common::component::PrivateDatabase::instance());

        void save(const QSharedPointer<domain::PrivateChatSession>& privateChatSession) override;
        QSharedPointer<domain::PrivateChatSession> of(const QString& privateChatSessionId) override;
        QSharedPointer<domain::PrivateChatSession> ofFriendShipId(const QString& friendShipId) override;
        QList<QSharedPointer<domain::PrivateChatSession>> ofAll(const QSet<QString>& privateChatSessionIds) override;

    private:
        common::component::IPrivateDatabase* privateDatabase = nullptr;
    };
}
