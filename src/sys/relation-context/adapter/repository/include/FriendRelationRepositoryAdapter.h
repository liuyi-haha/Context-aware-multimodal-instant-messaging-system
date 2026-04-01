#pragma once

#include "sys/common/component/Database.h"
#include "sys/relation-context/port/repository/include/FriendShipRepository.h"

namespace sys::relation::adapter
{
    class FriendShipRepositoryAdapter final : public port::FriendShipRepository
    {
    public:
        QSharedPointer<domain::FriendShip> of(const QString& friendShipId) override;

        explicit FriendShipRepositoryAdapter(
            common::component::IPrivateDatabase* privateDatabase = common::component::PrivateDatabase::instance());

        QSharedPointer<domain::FriendShip> of(const QString& ownerUserId, const QString& peerUserId) override;
        void save(const QSharedPointer<domain::FriendShip>& friendShip) override;

    private:
        common::component::IPrivateDatabase* privateDatabase = nullptr;
    };
}
