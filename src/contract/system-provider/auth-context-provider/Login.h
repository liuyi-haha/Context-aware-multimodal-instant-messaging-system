//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>

#include "contract/BaseResponse.hpp"

namespace contract::auth
{
    struct LoginRequest
    {
        QString account; // 可能是用户ID,也可能是手机号
        QString password;
    };

    struct LoginResponse : public contract::BaseResponse
    {
        QString userId;
    };
}
