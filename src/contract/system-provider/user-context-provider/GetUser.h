//
// Created by 86150 on 2026/3/25.
//
#pragma once
#include <QString>

#include "contract/BaseResponse.hpp"
#include "contract/system-provider/user-context-provider/UserView.hpp"

namespace contract::user
{
    struct GetUserRequest
    {
        QString userId;
    };

    struct GetUserResponse : public BaseResponse
    {
        UserView userView;
    };
}
