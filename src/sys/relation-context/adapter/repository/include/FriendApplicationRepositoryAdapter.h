#pragma once

#include "sys/common/component/Database.h"
#include "sys/relation-context/port/repository/include/FriendApplicationRepository.h"

namespace sys::relation::adapter
{
    class FriendApplicationRepositoryAdapter final : public port::FriendApplicationRepository
    {
    public:
        explicit FriendApplicationRepositoryAdapter(
            common::component::IPrivateDatabase* privateDatabase = common::component::PrivateDatabase::instance());

        QList<QSharedPointer<domain::FriendApplication>> ofApplicantUserId(const QString& applicantUserId) override;
        QList<QSharedPointer<domain::FriendApplication>> ofTargetUserId(const QString& ofTargetUserId) override;

        QSharedPointer<domain::FriendApplication> of(const QString& applicantUserId,
                                                     const QString& targetUserId,
                                                     domain::ApplicationStatus status) override;
        QSharedPointer<domain::FriendApplication> of(const QString& friendApplicationId) override;
        void save(QSharedPointer<domain::FriendApplication> friendApplication) override;

    private:
        common::component::IPrivateDatabase* privateDatabase = nullptr;
    };
}
