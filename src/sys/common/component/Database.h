#pragma once
#include <QSqlDatabase>
#include <qstandardpaths.h>
#include <QDebug>

#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/exception/InfraExcception.h"

namespace sys::common::component
{
    //static QString DATA_PATH = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    static QString DATA_PATH = "F:/GraduationProject/data";

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
            database.setDatabaseName(DATA_PATH + "/common.db");
            if (!database.open())
            {
                qDebug() << "数据库连接失败";
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
            QString dataPath = DATA_PATH + "/" + currentUserId;
            database.setDatabaseName(dataPath + "private.db");
            if (!database.open())
            {
                qDebug() << "数据库连接失败";
                throw core::InfraException("数据库连接失败");
            }
        }

        PrivateDatabase(const PrivateDatabase&) = delete;
        PrivateDatabase& operator=(const PrivateDatabase&) = delete;

    private:
        QSqlDatabase database;
    };
}
