//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>

#include "contract/BaseResponse.hpp"

namespace contract::user
{
    struct RegisterRequest
    {
        QString nickname;
        QString phone;
        QString password;
        QByteArray avatar;
    };

    struct RegisterResponse : public BaseResponse
    {
        std::optional<QString> userId;
    };
}
