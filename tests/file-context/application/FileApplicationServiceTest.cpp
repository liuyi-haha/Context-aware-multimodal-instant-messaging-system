//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <QByteArray>
#include <QFile>
#include <gtest/gtest.h>

#include "sys/common/component/FileUtil.h"
#include "sys/common/exception/InfraExcception.h"
#include "sys/file-context/application/service/include/FileApplicationService.h"
#include "utils/ExceptionTestUtils.h"

class FileApplicationServiceTest : public testing::Test
{
protected:
    void SetUp() override
    {
        applicationService = std::make_unique<FileApplicationService>();
    }

    void TearDown() override
    {
        applicationService.reset();
    }

    static void removeIfExists(const QString& fileId)
    {
        const sys::common::component::FileUtil fileUtil;
        QFile::remove(fileUtil.filePathOf(fileId));
    }

    std::unique_ptr<FileApplicationService> applicationService;
};

//rule:
// 上传的文件不能超过10MB
//---------------------- 上传文件 ----------------------------
TEST_F(FileApplicationServiceTest, 上传文件时_上传的文件如果超过限制大小_应该抛出异常)
{
    // 构造一个10MB + 1的文件内容
    QByteArray largeFileContent(10 * 1024 * 1024 + 1, 'a'); // 10MB + 1字节
    tests::utils::expectThrowWithMessage<sys::core::InfraException>([&]()
    {
        applicationService->uploadFile("file_id", largeFileContent);
    }, "上传的文件大小超过限制，最大允许10MB");
}

TEST_F(FileApplicationServiceTest, 上传文件时_上传的文件如果没有超过限制大小_应该保存在本地磁盘)
{
    const QString fileId = "file_id_upload_10mb";
    removeIfExists(fileId);

    // 构造一个10MB的文件内容
    QByteArray midLargeContent(10 * 1024 * 1024, 'a'); // 10MB字节

    applicationService->uploadFile(fileId, midLargeContent);

    // 访问磁盘，验证文件内容被正确保存
    EXPECT_EQ(applicationService->getFile(fileId), midLargeContent);

    removeIfExists(fileId);
}

TEST_F(FileApplicationServiceTest, 上传文件时_如果本地已经有同名的文件_应该不会被覆盖)
{
    const QString fileId = "file_id_not_override";
    removeIfExists(fileId);

    applicationService->uploadFile(fileId, "aaa");
    applicationService->uploadFile(fileId, "bbb");
    auto content = applicationService->getFile(fileId);
    EXPECT_EQ(content, "aaa");

    removeIfExists(fileId);
}

TEST_F(FileApplicationServiceTest, 上传文件时_如果fileId为空_应该抛出异常)
{
    tests::utils::expectThrowWithMessage<sys::core::InfraException>([&]()
    {
        applicationService->uploadFile("", "abc");
    }, "上传文件失败：fileId不能为空");
}


//---------------------- 下载文件 ----------------------------
TEST_F(FileApplicationServiceTest, 下载文件时_如果文件不存在_应该返回空数据)
{
    EXPECT_TRUE(applicationService->getFile("file_id_not_exists").isEmpty());
}

TEST_F(FileApplicationServiceTest, 下载文件时_上传过的文件应该能正确下载)
{
    const QString fileId = "file_id_download_uploaded";
    removeIfExists(fileId);

    const QByteArray expected = "this is a file";
    applicationService->uploadFile(fileId, expected);

    EXPECT_EQ(applicationService->getFile(fileId), expected);

    removeIfExists(fileId);
}

TEST_F(FileApplicationServiceTest, 下载文件时_如果fileId为空_应该返回空数据)
{
    EXPECT_TRUE(applicationService->getFile("").isEmpty());
}
