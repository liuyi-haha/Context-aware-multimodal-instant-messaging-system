#include "../include/FriendApplicationRepositoryAdapter.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "sys/common/exception/InfraExcception.h"

namespace sys::relation::adapter
{
    namespace
    {
        void ensureFriendApplicationTableReady(const QSqlDatabase& db)
        {
            if (!db.isOpen())
            {
                throw core::InfraException("数据库连接失败");
            }

            QSqlQuery query(db);
            if (!query.exec(
                "CREATE TABLE IF NOT EXISTS friend_application ("
                "id TEXT PRIMARY KEY,"
                "applicant_user_id TEXT NOT NULL,"
                "target_user_id TEXT NOT NULL,"
                "verification_message TEXT NOT NULL,"
                "recipient_remark TEXT NOT NULL,"
                "apply_time TEXT NOT NULL,"
                "application_status INTEGER NOT NULL"
                ")"))
            {
                throw core::InfraException("好友申请表初始化失败: " + query.lastError().text());
            }
        }

        int statusToInt(const domain::ApplicationStatus status)
        {
            switch (status)
            {
            case domain::ApplicationStatus::Accepted:
                return 0;
            case domain::ApplicationStatus::Rejected:
                return 1;
            case domain::ApplicationStatus::Pending:
            default:
                return 2;
            }
        }

        domain::ApplicationStatus intToStatus(const int status)
        {
            switch (status)
            {
            case 0:
                return domain::ApplicationStatus::Accepted;
            case 1:
                return domain::ApplicationStatus::Rejected;
            case 2:
            default:
                return domain::ApplicationStatus::Pending;
            }
        }

        QDateTime parseApplyTime(const QString& raw)
        {
            QDateTime parsed = QDateTime::fromString(raw, Qt::ISODateWithMs);
            if (!parsed.isValid())
            {
                parsed = QDateTime::fromString(raw, Qt::ISODate);
            }
            return parsed;
        }

        QSharedPointer<domain::FriendApplication> buildApplicationFromQuery(const QSqlQuery& query)
        {
            const QString id = query.value(0).toString();
            const QString applicantUserId = query.value(1).toString();
            const QString targetUserId = query.value(2).toString();
            const QString verificationMessage = query.value(3).toString();
            const QString recipientRemark = query.value(4).toString();
            const QString rawApplyTime = query.value(5).toString();
            const int statusValue = query.value(6).toInt();

            auto application = domain::FriendApplication::of(
                applicantUserId,
                id,
                targetUserId,
                verificationMessage,
                recipientRemark,
                parseApplyTime(rawApplyTime));

            const auto status = intToStatus(statusValue);
            if (status == domain::ApplicationStatus::Accepted)
            {
                application->accept();
            }
            else if (status == domain::ApplicationStatus::Rejected)
            {
                application->reject();
            }

            return application;
        }
    }

    FriendApplicationRepositoryAdapter::FriendApplicationRepositoryAdapter(
        common::component::IPrivateDatabase* privateDatabase)
        : privateDatabase(privateDatabase)
    {
        if (this->privateDatabase == nullptr)
        {
            throw core::InfraException("PrivateDatabase 未配置");
        }
        ensureFriendApplicationTableReady(this->privateDatabase->getDataBase());
    }

    QList<QSharedPointer<domain::FriendApplication>> FriendApplicationRepositoryAdapter::ofApplicantUserId(
        const QString& applicantUserId)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureFriendApplicationTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT id, applicant_user_id, target_user_id, verification_message, recipient_remark, apply_time, application_status "
            "FROM friend_application WHERE applicant_user_id = :applicant_user_id "
            "ORDER BY apply_time DESC");
        query.bindValue(":applicant_user_id", applicantUserId);

        if (!query.exec())
        {
            throw core::InfraException("查询申请人好友申请失败: " + query.lastError().text());
        }

        QList<QSharedPointer<domain::FriendApplication>> result;
        while (query.next())
        {
            result.push_back(buildApplicationFromQuery(query));
        }
        return result;
    }

    QList<QSharedPointer<domain::FriendApplication>> FriendApplicationRepositoryAdapter::ofTargetUserId(
        const QString& ofTargetUserId)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureFriendApplicationTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT id, applicant_user_id, target_user_id, verification_message, recipient_remark, apply_time, application_status "
            "FROM friend_application WHERE target_user_id = :target_user_id "
            "ORDER BY apply_time DESC");
        query.bindValue(":target_user_id", ofTargetUserId);

        if (!query.exec())
        {
            throw core::InfraException("查询目标用户好友申请失败: " + query.lastError().text());
        }

        QList<QSharedPointer<domain::FriendApplication>> result;
        while (query.next())
        {
            result.push_back(buildApplicationFromQuery(query));
        }
        return result;
    }

    QSharedPointer<domain::FriendApplication> FriendApplicationRepositoryAdapter::of(
        const QString& applicantUserId,
        const QString& targetUserId,
        const domain::ApplicationStatus status)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureFriendApplicationTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT id, applicant_user_id, target_user_id, verification_message, recipient_remark, apply_time, application_status "
            "FROM friend_application "
            "WHERE applicant_user_id = :applicant_user_id "
            "AND target_user_id = :target_user_id "
            "AND application_status = :application_status "
            "ORDER BY apply_time DESC LIMIT 1");
        query.bindValue(":applicant_user_id", applicantUserId);
        query.bindValue(":target_user_id", targetUserId);
        query.bindValue(":application_status", statusToInt(status));

        if (!query.exec())
        {
            throw core::InfraException("查询好友申请失败: " + query.lastError().text());
        }

        if (!query.next())
        {
            return nullptr;
        }

        return buildApplicationFromQuery(query);
    }

    QSharedPointer<domain::FriendApplication> FriendApplicationRepositoryAdapter::of(const QString& friendApplicationId)
    {
        QSqlDatabase db = privateDatabase->getDataBase();
        ensureFriendApplicationTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "SELECT id, applicant_user_id, target_user_id, verification_message, recipient_remark, apply_time, application_status "
            "FROM friend_application WHERE id = :id LIMIT 1");
        query.bindValue(":id", friendApplicationId);

        if (!query.exec())
        {
            throw core::InfraException("按ID查询好友申请失败: " + query.lastError().text());
        }

        if (!query.next())
        {
            return nullptr;
        }

        return buildApplicationFromQuery(query);
    }

    void FriendApplicationRepositoryAdapter::save(QSharedPointer<domain::FriendApplication> friendApplication)
    {
        if (friendApplication == nullptr)
        {
            return;
        }

        QSqlDatabase db = privateDatabase->getDataBase();
        ensureFriendApplicationTableReady(db);

        QSqlQuery query(db);
        query.prepare(
            "INSERT OR REPLACE INTO friend_application ("
            "id, applicant_user_id, target_user_id, verification_message, recipient_remark, apply_time, application_status"
            ") VALUES ("
            ":id, :applicant_user_id, :target_user_id, :verification_message, :recipient_remark, :apply_time, :application_status"
            ")");
        query.bindValue(":id", friendApplication->friendApplicationId());
        query.bindValue(":applicant_user_id", friendApplication->applicantUserId());
        query.bindValue(":target_user_id", friendApplication->targetUserId());
        query.bindValue(":verification_message", friendApplication->verificationMessageValue());
        query.bindValue(":recipient_remark", friendApplication->recipientRemarkValue());
        query.bindValue(":apply_time", friendApplication->applyTime().toString(Qt::ISODateWithMs));
        query.bindValue(":application_status", statusToInt(friendApplication->applicationStatus()));

        if (!query.exec())
        {
            throw core::InfraException("保存好友申请失败: " + query.lastError().text());
        }
    }
}
