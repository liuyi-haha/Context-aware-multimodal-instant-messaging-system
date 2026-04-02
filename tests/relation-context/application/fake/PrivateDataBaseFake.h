#pragma once

#include <QSqlDatabase>
#include <QUuid>

#include "sys/common/component/Database.h"

namespace tests::common::fake
{
    class PrivateDataBaseFake : public sys::common::component::IPrivateDatabase
    {
    public:
        PrivateDataBaseFake()
        {
            connectionName = QStringLiteral("test-private-db-") + QUuid::createUuid().toString(QUuid::WithoutBraces);
            database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
            database.setDatabaseName(":memory:");
            database.open();
        }

        ~PrivateDataBaseFake() override
        {
            if (database.isOpen())
            {
                database.close();
            }
            const QString name = connectionName;
            database = QSqlDatabase();
            QSqlDatabase::removeDatabase(name);
        }

        QSqlDatabase* getDataBase() override
        {
            return &database;
        }

    private:
        QString connectionName;
        QSqlDatabase database;
    };
}

