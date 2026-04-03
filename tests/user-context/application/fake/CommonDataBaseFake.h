//
// Created by 86150 on 2026/3/28.
//

#pragma once

#include <QSqlDatabase>

#include "sys/common/component/Database.h"

namespace tests::user::fake
{
    class CommonDataBaseFake : public sys::common::component::ICommonDatabase
    {
    public:
        CommonDataBaseFake()
        {
            database = QSqlDatabase::addDatabase("QSQLITE", "test-common-db-connection");
            database.setDatabaseName(":memory:");
            database.open();
        }

        QSqlDatabase getDataBase() override
        {
            return database;
        }

    private:
        QSqlDatabase database;
    };
}
