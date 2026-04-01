//
// Created by 86150 on 2026/3/25.
//
#pragma once
#include <QString>

#include "contract/BaseResponse.hpp"

namespace contract::relation
{
    struct SendFriendApplicationRequest
    {
        QString targetUserId;
        QString verificationMessage;
        QString recipientRemark; //对方备注
    };

    struct SendFriendApplicationResponse : BaseResponse
    {
        QString friendApplicationId;
    };
}
