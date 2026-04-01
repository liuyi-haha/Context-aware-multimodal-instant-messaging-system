#include "../include/FriendRelationRepositoryAdapter.h"

#include <QSqlError>
#include <QSqlQuery>

#include "sys/common/exception/InfraExcception.h"

namespace sys::relation::adapter
{
    namespace
    {
        void ensureFriendShipTableReady(QSqlDatabase* db)
        {
            if (db == nullptr || !db->isOpen())
            {
                throw core::InfraException("数据库连接失败");
            }

            QSqlQuery query(*db);
            if (!query.exec(
                "CREATE TABLE IF NOT EXISTS friend_ship ("
                "friend_ship_id TEXT PRIMARY KEY,"
                "owner_user_id TEXT NOT NULL,"
                "peer_user_id TEXT NOT NULL"
                ")"))
            {
                throw core::InfraException("好友关系表初始化失败: " + query.lastError().text());
            }
        }
    }

    QSharedPointer<domain::FriendShip> FriendShipRepositoryAdapter::of(const QString& friendShipId)
    {
        if (friendShipId.isEmpty())
        {
            return nullptr;
        }

        QSqlDatabase* db = privateDatabase->getDataBase();
        ensureFriendShipTableReady(db);

        QSqlQuery query(*db);
        query.prepare(
            "SELECT friend_ship_id, owner_user_id, peer_user_id "
            "FROM friend_ship WHERE friend_ship_id = :friend_ship_id LIMIT 1");
        query.bindValue(":friend_ship_id", friendShipId);

        if (!query.exec())
        {
            throw core::InfraException("按好友关系ID查询好友关系失败: " + query.lastError().text());
        }

        if (!query.next())
        {
            return nullptr;
        }

        return domain::FriendShip::of(
            query.value(0).toString(),
            query.value(1).toString(),
            query.value(2).toString());
    }

    FriendShipRepositoryAdapter::FriendShipRepositoryAdapter(common::component::IPrivateDatabase* privateDatabase)
        : privateDatabase(privateDatabase)
    {
        if (this->privateDatabase == nullptr)
        {
            throw core::InfraException("PrivateDatabase 未配置");
        }
        ensureFriendShipTableReady(this->privateDatabase->getDataBase());
    }

    QSharedPointer<domain::FriendShip> FriendShipRepositoryAdapter::of(const QString& ownerUserId,
                                                                       const QString& peerUserId)
    {
        QSqlDatabase* db = privateDatabase->getDataBase();
        ensureFriendShipTableReady(db);

        QSqlQuery query(*db);
        query.prepare(
            "SELECT friend_ship_id, owner_user_id, peer_user_id "
            "FROM friend_ship WHERE owner_user_id = :owner_user_id and peer_user_id = :peer_user_id LIMIT 1");
        query.bindValue(":owner_user_id", ownerUserId);
        query.bindValue(":peer_user_id", peerUserId);

        if (!query.exec())
        {
            throw core::InfraException("查询好友关系失败: " + query.lastError().text());
        }

        if (!query.next())
        {
            return nullptr;
        }

        return domain::FriendShip::of(
            query.value(0).toString(),
            query.value(1).toString(),
            query.value(2).toString());
    }

    void FriendShipRepositoryAdapter::save(const QSharedPointer<domain::FriendShip>& friendShip)
    {
        if (friendShip == nullptr)
        {
            return;
        }

        QSqlDatabase* db = privateDatabase->getDataBase();
        ensureFriendShipTableReady(db);

        QSqlQuery query(*db);
        query.prepare(
            "INSERT OR REPLACE INTO friend_ship ("
            "friend_ship_id, owner_user_id, peer_user_id"
            ") VALUES ("
            ":friend_ship_id, :owner_user_id, :peer_user_id"
            ")");
        query.bindValue(":friend_ship_id", friendShip->friendShipId());
        query.bindValue(":owner_user_id", friendShip->ownerUserId());
        query.bindValue(":peer_user_id", friendShip->peerUserId());

        if (!query.exec())
        {
            throw core::InfraException("保存好友关系失败: " + query.lastError().text());
        }
    }
}
