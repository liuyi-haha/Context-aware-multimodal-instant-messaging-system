//
// Created by 86150 on 2026/3/27.
//

#pragma once
#include "contract/BaseResponse.hpp"

namespace contract::relation
{
    struct RejectFriendApplicationRequest
    {
        QString friendApplicationId;
    };

    struct RejectFriendApplicationResponse : public BaseResponse
    {
    };
}
