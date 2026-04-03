#include "../include/UserRepositoryAdapter.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "sys/common/exception/InfraExcception.h"

namespace sys::user::adapter
{
    namespace
    {
        void ensureUserTableReady(const QSqlDatabase& db)
        {
            if (!db.isOpen())
            {
                throw core::InfraException("数据库连接失败");
            }

            QSqlQuery query(db);
            if (!query.exec(
                "CREATE TABLE IF NOT EXISTS user ("
                "id TEXT PRIMARY KEY,"
                "nickname TEXT NOT NULL,"
                "avatar_file_id TEXT NOT NULL,"
                "phone TEXT NOT NULL UNIQUE,"
                "description TEXT NOT NULL"
                ")"))
            {
                throw core::InfraException("用户表初始化失败: " + query.lastError().text());
            }
        }

        QString normalizeNickname(const QString& nickname)
        {
            return nickname.isEmpty() ? QStringLiteral("默认昵称") : nickname;
        }

        QString normalizePhone(const QString& phone)
        {
            return phone.isEmpty() ? QStringLiteral("13800000000") : phone;
        }
    }

    QSharedPointer<domain::User> UserPO::toModel() const
    {
        if (id.isEmpty())
        {
            return nullptr;
        }

        return domain::User::of(
            id,
            normalizeNickname(nickname),
            normalizePhone(phone),
            QByteArray(),
            avatarFileId,
            description
        );
    }

    void UserPO::fromModel(const QSharedPointer<domain::User>& user)
    {
        if (user == nullptr)
        {
            return;
        }

        id = user->userId();
        nickname = user->nicknameValue();
        avatarFileId = user->avatarFileId();
        phone = user->phoneValue();
        description = user->descriptionValue();
    }

    UserRepositoryAdapter::UserRepositoryAdapter(common::component::ICommonDatabase* commonDatabase)
        : commonDatabase(commonDatabase)
    {
        if (this->commonDatabase == nullptr)
        {
            throw core::InfraException("CommonDatabase 未配置");
        }
        ensureUserTableReady(this->commonDatabase->getDataBase());
    }

    QSharedPointer<domain::User> UserRepositoryAdapter::of(const QString& userId)
    {
        if (userId.isEmpty())
        {
            return nullptr;
        }

        const UserPO user = findById(userId);
        return user.toModel();
    }

    QList<QSharedPointer<domain::User>> UserRepositoryAdapter::ofAll(const QSet<QString>& userIds)
    {
        QList<QSharedPointer<domain::User>> users;
        for (const auto& userId : userIds)
        {
            if (userId.isEmpty())
            {
                continue;
            }

            const auto user = findById(userId).toModel();
            if (user != nullptr)
            {
                users.append(user);
            }
        }

        return users;
    }

    void UserRepositoryAdapter::save(QSharedPointer<domain::User> user)
    {
        if (user == nullptr)
        {
            return;
        }

        UserPO po;
        po.fromModel(user);
        if (po.id.isEmpty())
        {
            return;
        }

        const UserPO existing = findById(po.id);
        if (existing.id.isEmpty())
        {
            insert(po);
        }
        else
        {
            update(po);
        }
    }

    void UserRepositoryAdapter::insert(const UserPO& user)
    {
        QSqlDatabase db = commonDatabase->getDataBase();
        ensureUserTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "INSERT INTO user (id, nickname, avatar_file_id, phone, description) "
            "VALUES (:id, :nickname, :avatar_file_id, :phone, :description)"
        );
        query.bindValue(":id", user.id);
        query.bindValue(":nickname", normalizeNickname(user.nickname));
        query.bindValue(":avatar_file_id", user.avatarFileId);
        query.bindValue(":phone", normalizePhone(user.phone));
        query.bindValue(":description", user.description);

        if (!query.exec())
        {
            throw core::InfraException("新增用户失败: " + query.lastError().text());
        }
    }

    void UserRepositoryAdapter::update(const UserPO& user)
    {
        QSqlDatabase db = commonDatabase->getDataBase();
        ensureUserTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "UPDATE user SET "
            "nickname = :nickname, "
            "avatar_file_id = :avatar_file_id, "
            "phone = :phone, "
            "description = :description "
            "WHERE id = :id"
        );
        query.bindValue(":id", user.id);
        query.bindValue(":nickname", normalizeNickname(user.nickname));
        query.bindValue(":avatar_file_id", user.avatarFileId);
        query.bindValue(":phone", normalizePhone(user.phone));
        query.bindValue(":description", user.description);

        if (!query.exec())
        {
            throw core::InfraException("更新用户失败: " + query.lastError().text());
        }
    }

    UserPO UserRepositoryAdapter::findById(const QString& userId)
    {
        QSqlDatabase db = commonDatabase->getDataBase();
        ensureUserTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT id, nickname, avatar_file_id, phone, description "
            "FROM user WHERE id = :id"
        );
        query.bindValue(":id", userId);

        if (!query.exec())
        {
            throw core::InfraException("查询用户失败: " + query.lastError().text());
        }

        if (!query.next())
        {
            return {};
        }

        UserPO po;
        po.id = query.value(0).toString();
        po.nickname = query.value(1).toString();
        po.avatarFileId = query.value(2).toString();
        po.phone = query.value(3).toString();
        po.description = query.value(4).toString();
        return po;
    }
}
