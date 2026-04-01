//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <optional>
#include <QString>

namespace contract
{
    struct BaseResponse
    {
        bool success;
        std::optional<QString> errMsg;
    };
}