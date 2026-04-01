#pragma once
#include <QSqlDatabase>
#include <qstandardpaths.h>

#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"

namespace sys::common::component
{
    class ICommonDatabase
    {
    public:
        virtual ~ICommonDatabase() = default;
        virtual QSqlDatabase* getDataBase() = 0;
    };

    class CommonDatabase : public ICommonDatabase
    {
    public:
        static CommonDatabase* instance()
        {
            static CommonDatabase singleton;
            return &singleton;
        }

        QSqlDatabase* getDataBase() override
        {
            return &database;
        }

    private:
        CommonDatabase()
        {
            database = QSqlDatabase::addDatabase("QSQLITE", "common-db-connection");
            const QString dataPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
            database.setDatabaseName(dataPath + "/common.db");
            if (!database.open())
            {
                throw core::InfraException("数据库连接失败");
            }
        }

        CommonDatabase(const CommonDatabase&) = delete;
        CommonDatabase& operator=(const CommonDatabase&) = delete;

    private:
        QSqlDatabase database;
    };

    class IPrivateDatabase
    {
    public:
        virtual ~IPrivateDatabase() = default;
        virtual QSqlDatabase* getDataBase() = 0;
    };

    class PrivateDatabase : public IPrivateDatabase
    {
    public:
        static PrivateDatabase* instance()
        {
            static PrivateDatabase singleton;
            return &singleton;
        }

        QSqlDatabase* getDataBase() override
        {
            return &database;
        }

    private:
        PrivateDatabase()
        {
            const QString currentUserId = UserCredentialManager::instance().getCurrentUserId();
            const QString connectionName = QString("private-db-") + currentUserId;
            database = QSqlDatabase::addDatabase("QSQLITE", connectionName);

            const QString dataPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
            database.setDatabaseName(dataPath + currentUserId + ".db");
            if (!database.open())
            {
                throw core::InfraException("数据库连接失败");
            }
        }

        PrivateDatabase(const PrivateDatabase&) = delete;
        PrivateDatabase& operator=(const PrivateDatabase&) = delete;

    private:
        QSqlDatabase database;
    };
}
