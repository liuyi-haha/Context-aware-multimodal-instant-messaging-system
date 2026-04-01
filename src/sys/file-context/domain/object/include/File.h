//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QString>

#include "sys/common/FileSize.hpp"

namespace sys::file::domain
{
    enum class FileType
    {
        Png,
        Pdf,
        Txt
    };

    class File
    {
    private:
        QString fileId;
        FileType fileType = FileType::Txt;
        core::FileSize fileSize;
    };
}
