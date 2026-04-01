#pragma once

#include "sys/common/component/Database.h"
#include "sys/user-context/port/repository/include/UserRepository.h"

namespace sys::user::adapter
{
    struct UserPO
    {
        QString id;
        QString nickname;
        QString avatarFileId;
        QString phone;
        QString description;

        QSharedPointer<domain::User> toModel() const;
        void fromModel(const QSharedPointer<domain::User>& user);
    };

    // User数据是多个用户共用的
    class UserRepositoryAdapter final : public port::UserRepository
    {
    public:
        explicit UserRepositoryAdapter(
            common::component::ICommonDatabase* commonDatabase = common::component::CommonDatabase::instance());

        void save(QSharedPointer<domain::User> user) override;
        QSharedPointer<domain::User> of(const QString& userId) override;
        QList<QSharedPointer<domain::User>> ofAll(const QSet<QString>& userIds) override;

    private:
        common::component::ICommonDatabase* commonDatabase = nullptr;

    private:
        void insert(const UserPO& user);
        void update(const UserPO& user);
        UserPO findById(const QString& userId);
    };
}
