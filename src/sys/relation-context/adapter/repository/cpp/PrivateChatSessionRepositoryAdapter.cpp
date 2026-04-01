#include "../include/PrivateChatSessionRepositoryAdapter.h"

#include <QSqlError>
#include <QSqlQuery>

#include "sys/common/exception/InfraExcception.h"

namespace sys::relation::adapter
{
    void ensurePrivateChatSessionTableReady(QSqlDatabase* db)
    {
        if (db == nullptr || !db->isOpen())
        {
            throw core::InfraException("数据库连接失败");
        }

        QSqlQuery query(*db);
        if (!query.exec(
            "CREATE TABLE IF NOT EXISTS private_chat_session ("
            "private_chat_session_id TEXT PRIMARY KEY,"
            "friend_ship_id TEXT NOT NULL"
            ")"))
        {
            throw core::InfraException("私聊会话表初始化失败: " + query.lastError().text());
        }
    }

    PrivateChatSessionRepositoryAdapter::PrivateChatSessionRepositoryAdapter(
        common::component::IPrivateDatabase* privateDatabase)
        : privateDatabase(privateDatabase)
    {
        if (this->privateDatabase == nullptr)
        {
            throw core::InfraException("PrivateDatabase 未配置");
        }
        ensurePrivateChatSessionTableReady(this->privateDatabase->getDataBase());
    }

    void PrivateChatSessionRepositoryAdapter::save(
        const QSharedPointer<domain::PrivateChatSession>& privateChatSession)
    {
        if (privateChatSession == nullptr)
        {
            return;
        }

        QSqlDatabase* db = privateDatabase->getDataBase();

        QSqlQuery query(*db);
        query.prepare(
            "INSERT OR REPLACE INTO private_chat_session ("
            "private_chat_session_id, friend_ship_id"
            ") VALUES ("
            ":private_chat_session_id, :friend_ship_id"
            ")");
        query.bindValue(":private_chat_session_id", privateChatSession->privateChatSessionId());
        query.bindValue(":friend_ship_id", privateChatSession->friendShipId());

        if (!query.exec())
        {
            throw core::InfraException("保存私聊会话失败: " + query.lastError().text());
        }
    }

    QSharedPointer<domain::PrivateChatSession> PrivateChatSessionRepositoryAdapter::of(
        const QString& privateChatSessionId)
    {
        QSqlDatabase* db = privateDatabase->getDataBase();

        QSqlQuery query(*db);
        query.prepare(
            "SELECT private_chat_session_id, friend_ship_id "
            "FROM private_chat_session WHERE private_chat_session_id = :id LIMIT 1");
        query.bindValue(":id", privateChatSessionId);

        if (!query.exec())
        {
            throw core::InfraException("查询私聊会话失败: " + query.lastError().text());
        }
        if (!query.next())
        {
            return nullptr;
        }

        return domain::PrivateChatSession::of(query.value(0).toString(), query.value(1).toString());
    }

    QSharedPointer<domain::PrivateChatSession> PrivateChatSessionRepositoryAdapter::ofFriendShipId(
        const QString& friendShipId)
    {
        QSqlDatabase* db = privateDatabase->getDataBase();

        QSqlQuery query(*db);
        query.prepare(
            "SELECT private_chat_session_id, friend_ship_id "
            "FROM private_chat_session WHERE friend_ship_id = :friend_ship_id LIMIT 1");
        query.bindValue(":friend_ship_id", friendShipId);

        if (!query.exec())
        {
            throw core::InfraException("按好友关系查询私聊会话失败: " + query.lastError().text());
        }
        if (!query.next())
        {
            return nullptr;
        }

        return domain::PrivateChatSession::of(query.value(0).toString(), query.value(1).toString());
    }

    QList<QSharedPointer<domain::PrivateChatSession>> PrivateChatSessionRepositoryAdapter::ofAll(
        const QSet<QString>& privateChatSessionIds)
    {
        if (privateChatSessionIds.isEmpty())
        {
            return QList<QSharedPointer<domain::PrivateChatSession>>();
        }

        QSqlDatabase* db = privateDatabase->getDataBase();
        ensurePrivateChatSessionTableReady(db);

        // 构建 IN 子句
        QStringList idList;
        for (const auto& id : privateChatSessionIds)
        {
            idList.append("'" + id + "'");
        }
        QString inClause = idList.join(",");

        QSqlQuery query(*db);
        QString sql = QString(
            "SELECT private_chat_session_id, friend_ship_id "
            "FROM private_chat_session "
            "WHERE private_chat_session_id IN (%1)").arg(inClause);

        if (!query.exec(sql))
        {
            throw core::InfraException("批量查询私聊会话失败: " + query.lastError().text());
        }

        QList<QSharedPointer<domain::PrivateChatSession>> sessions;
        while (query.next())
        {
            sessions.append(domain::PrivateChatSession::of(
                query.value(0).toString(),
                query.value(1).toString()));
        }

        return sessions;
    }
}
