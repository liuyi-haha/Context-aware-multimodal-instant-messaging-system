#pragma once
#include <QSqlDatabase>
#include <QDebug>
#include <QDir>
#include <QThread>

#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"

namespace sys::common::component
{
    static QString DATA_PATH = "F:/GraduationProject/data";

    class ICommonDatabase
    {
    public:
        virtual ~ICommonDatabase() = default;
        virtual QSqlDatabase getDataBase() = 0;
    };

    class CommonDatabase : public ICommonDatabase
    {
    public:
        static CommonDatabase* instance()
        {
            static CommonDatabase singleton;
            return &singleton;
        }

        QSqlDatabase getDataBase() override
        {
            thread_local QString connectionName;
            thread_local QSqlDatabase database;
            const QString expectedConnectionName = QString("common-db-%1")
                .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));

            if (connectionName != expectedConnectionName || !database.isValid())
            {
                connectionName = expectedConnectionName;
                database = QSqlDatabase::contains(connectionName)
                               ? QSqlDatabase::database(connectionName)
                               : QSqlDatabase::addDatabase("QSQLITE", connectionName);
                database.setDatabaseName(DATA_PATH + "/common.db");
                if (!database.open())
                {
                    qDebug() << "数据库连接失败";
                    throw core::InfraException("数据库连接失败");
                }
            }
            return database;
        }

        CommonDatabase(const CommonDatabase&) = delete;
        CommonDatabase& operator=(const CommonDatabase&) = delete;

    private:
        CommonDatabase() = default;
    };

    class IPrivateDatabase
    {
    public:
        virtual ~IPrivateDatabase() = default;
        virtual QSqlDatabase getDataBase() = 0;
    };

    class PrivateDatabase : public IPrivateDatabase
    {
    public:
        static PrivateDatabase* instance()
        {
            static PrivateDatabase singleton;
            return &singleton;
        }

        QSqlDatabase getDataBase() override
        {
            const QString currentUserId = UserCredentialManager::instance().getCurrentUserId();
            const QString expectedConnectionName = QString("private-db-%1-%2")
                                                   .arg(currentUserId)
                                                   .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));

            thread_local QString connectionName;
            thread_local QSqlDatabase database;
            if (connectionName != expectedConnectionName || !database.isValid())
            {
                connectionName = expectedConnectionName;
                database = QSqlDatabase::contains(connectionName)
                               ? QSqlDatabase::database(connectionName)
                               : QSqlDatabase::addDatabase("QSQLITE", connectionName);

                const QString dataPath = DATA_PATH + "/" + currentUserId;
                (void)QDir().mkpath(dataPath);
                database.setDatabaseName(dataPath + "private.db");
                if (!database.open())
                {
                    qDebug() << "数据库连接失败";
                    throw core::InfraException("数据库连接失败");
                }
            }
            return database;
        }

        PrivateDatabase(const PrivateDatabase&) = delete;
        PrivateDatabase& operator=(const PrivateDatabase&) = delete;

    private:
        PrivateDatabase() = default;
    };
}
