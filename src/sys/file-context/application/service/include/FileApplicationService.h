//
// Created by 86150 on 2026/3/21.
//
#pragma once
#include <QByteArray>
#include <QObject>

class FileApplicationService : public QObject
{
    Q_OBJECT

public:
    explicit FileApplicationService(QObject* parent = nullptr);
    QByteArray getFile(const QString& fileId);
    void uploadFile(const QString& fileId, const QByteArray& fileData);
};
