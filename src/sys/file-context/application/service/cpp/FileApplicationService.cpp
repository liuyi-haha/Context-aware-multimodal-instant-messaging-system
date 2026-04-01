//
// Created by 86150 on 2026/3/21.
//

#include "../include/FileApplicationService.h"

#include "sys/common/component/FileUtil.h"
#include "sys/common/exception/InfraExcception.h"

namespace
{
    constexpr int kMaxUploadSizeInBytes = 10 * 1024 * 1024;
}

FileApplicationService::FileApplicationService(QObject* parent)
    : QObject(parent)
{
}

QByteArray FileApplicationService::getFile(const QString& fileId)
{
    if (fileId.isEmpty())
    {
        return {};
    }

    sys::common::component::FileUtil fileUtil;
    return fileUtil.readFile(fileId);
}

void FileApplicationService::uploadFile(const QString& fileId, const QByteArray& fileData)
{
    if (fileId.isEmpty())
    {
        throw sys::core::InfraException("上传文件失败：fileId不能为空");
    }

    if (fileData.size() > kMaxUploadSizeInBytes)
    {
        throw sys::core::InfraException("上传的文件大小超过限制，最大允许10MB");
    }

    sys::common::component::FileUtil fileUtil;
    fileUtil.saveFile(fileId, fileData);
}
