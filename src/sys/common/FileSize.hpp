//
// Created by 86150 on 2026/3/23.
//
#pragma once
namespace sys::core
{
    enum class FileSizeUnit
    {
        B,
        KB,
        MB,
        GB
    };

    class FileSize
    {
    private:
        const int value = 0;
        const FileSizeUnit unit;
    };
}