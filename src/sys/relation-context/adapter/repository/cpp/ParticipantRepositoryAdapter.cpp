#include "../include/ParticipantRepositoryAdapter.h"

#include <QSqlError>
#include <QSqlQuery>

#include "sys/common/exception/InfraExcception.h"

namespace sys::relation::adapter
{
    int roleToInt(const domain::ParticipantRole role)
    {
        switch (role)
        {
        case domain::ParticipantRole::GroupOwner:
            return 0;
        case domain::ParticipantRole::GroupMember:
            return 1;
        case domain::ParticipantRole::PrivateMember:
        default:
            return 2;
        }
    }

    domain::ParticipantRole intToRole(const int value)
    {
        switch (value)
        {
        case 0:
            return domain::ParticipantRole::GroupOwner;
        case 1:
            return domain::ParticipantRole::GroupMember;
        case 2:
        default:
            return domain::ParticipantRole::PrivateMember;
        }
    }

    struct ParticipantPO
    {
        QString participantId;
        QString chatSessionId;
        QString userId;
        int participantRole = 0;
        std::optional<QString> remark;

        QSharedPointer<domain::Participant> toModel()
        {
            if (remark.has_value())
            {
                return domain::Participant::of(participantId, chatSessionId, userId, intToRole(participantRole),
                                               remark.value());
            }

            return domain::Participant::of(participantId, chatSessionId, userId, intToRole(participantRole));
        }

        void from(const QSqlQuery& query)
        {
            participantId = query.value(0).toString();
            chatSessionId = query.value(1).toString();
            userId = query.value(2).toString();
            participantRole = query.value(3).toInt();
            if (!query.value("remark").isNull())
            {
                remark = query.value("remark").toString();
            }
        }
    };

    void ensureParticipantTableReady(const QSqlDatabase& db)
    {
        if (!db.isOpen())
        {
            throw core::InfraException("数据库连接失败");
        }

        QSqlQuery query(db);
        if (!query.exec(
            "CREATE TABLE IF NOT EXISTS participant ("
            "participant_id TEXT PRIMARY KEY,"
            "chat_session_id TEXT NOT NULL,"
            "user_id TEXT NOT NULL,"
            "role INTEGER NOT NULL,"
            "remark TEXT"
            ")"))
        {
            throw core::InfraException("会话成员表初始化失败: " + query.lastError().text());
        }
    }


    ParticipantRepositoryAdapter::ParticipantRepositoryAdapter(common::component::IPrivateDatabase* privateDatabase)
        : privateDatabase(privateDatabase)
    {
        if (this->privateDatabase == nullptr)
        {
            throw core::InfraException("PrivateDatabase 未配置");
        }
        ensureParticipantTableReady(this->privateDatabase->getDataBase());
    }

    void ParticipantRepositoryAdapter::saveAll(
        const QVector<QSharedPointer<domain::Participant>>& participantsToSave)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureParticipantTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "INSERT OR REPLACE INTO participant ("
            "participant_id, chat_session_id, user_id, role, remark"
            ") VALUES ("
            ":participant_id, :chat_session_id, :user_id, :role, :remark"
            ")");

        for (const auto& participant : participantsToSave)
        {
            if (participant == nullptr)
            {
                continue;
            }

            query.bindValue(":participant_id", participant->id());
            query.bindValue(":chat_session_id", participant->sessionId());
            query.bindValue(":user_id", participant->userIdValue());
            query.bindValue(":role", roleToInt(participant->roleValue()));
            query.bindValue(":remark", participant->hasRemark() ? participant->remarkValue() : QVariant());

            if (!query.exec())
            {
                throw core::InfraException("保存会话成员失败: " + query.lastError().text());
            }
        }
    }

    QSharedPointer<domain::Participant> ParticipantRepositoryAdapter::of(const QString& participantId)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureParticipantTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT participant_id, chat_session_id, user_id, role, remark "
            "FROM participant WHERE participant_id = :participant_id LIMIT 1");
        query.bindValue(":participant_id", participantId);

        if (!query.exec())
        {
            throw core::InfraException("查询会话成员失败: " + query.lastError().text());
        }
        if (!query.next())
        {
            return nullptr;
        }

        ParticipantPO po;
        po.from(query);
        return po.toModel();
    }

    QSharedPointer<domain::Participant> ParticipantRepositoryAdapter::of(
        const QString& privateChatSessionId,
        const QString& userId)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureParticipantTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT participant_id, chat_session_id, user_id, role, remark "
            "FROM participant "
            "WHERE chat_session_id = :chat_session_id AND user_id = :user_id LIMIT 1");
        query.bindValue(":chat_session_id", privateChatSessionId);
        query.bindValue(":user_id", userId);

        if (!query.exec())
        {
            throw core::InfraException("按会话和用户查询成员失败: " + query.lastError().text());
        }
        if (!query.next())
        {
            return nullptr;
        }

        ParticipantPO po;
        po.from(query);
        return po.toModel();
    }

    QList<QSharedPointer<domain::Participant>> ParticipantRepositoryAdapter::ofChatSessionId(
        const QString& chatSessionId)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureParticipantTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT participant_id, chat_session_id, user_id, role, remark "
            "FROM participant "
            "WHERE chat_session_id = :chat_session_id");
        query.bindValue(":chat_session_id", chatSessionId);

        if (!query.exec())
        {
            throw core::InfraException("按会话ID查询成员失败: " + query.lastError().text());
        }

        QList<QSharedPointer<domain::Participant>> participants;
        while (query.next())
        {
            ParticipantPO po;
            po.from(query);
            participants.append(po.toModel());
        }

        return participants;
    }

    QList<QSharedPointer<domain::Participant>> ParticipantRepositoryAdapter::ofAll(const QString& userId,
        domain::ParticipantRole role)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureParticipantTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT participant_id, chat_session_id, user_id, role, remark "
            "FROM participant "
            "WHERE user_id = :user_id AND role = :role");
        query.bindValue(":user_id", userId);
        query.bindValue(":role", roleToInt(role));

        if (!query.exec())
        {
            throw core::InfraException("按用户和角色查询成员失败: " + query.lastError().text());
        }

        QList<QSharedPointer<domain::Participant>> participants;
        while (query.next())
        {
            ParticipantPO po;
            po.from(query);
            participants.append(po.toModel());
        }
        return participants;
    }
}
