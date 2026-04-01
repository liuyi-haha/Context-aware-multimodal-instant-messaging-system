//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QString>

namespace sys::common::component
{
    class FileUtil
    {
    public:
        bool saveFile(const QString& fileId, const QByteArray& fileData)
        {
            if (fileId.isEmpty())
            {
                return false;
            }

            QDir dir(FILE_PATH);
            if (!dir.exists() && !dir.mkpath("."))
            {
                return false;
            }

            QFile file(filePathOf(fileId));
            if (file.exists())
            {
                return false;
            }

            if (!file.open(QIODevice::WriteOnly))
            {
                return false;
            }

            const qint64 written = file.write(fileData);
            file.close();
            return written == fileData.size();
        }

        QByteArray readFile(const QString& fileId)
        {
            if (fileId.isEmpty())
            {
                return {};
            }

            QFile file(filePathOf(fileId));
            if (!file.exists() || !file.open(QIODevice::ReadOnly))
            {
                return {};
            }

            const QByteArray content = file.readAll();
            file.close();
            return content;
        }

        QString filePathOf(const QString& fileId) const
        {
            return FILE_PATH + "/" + fileId;
        }

        const QString FILE_PATH = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/file";
    };
}
