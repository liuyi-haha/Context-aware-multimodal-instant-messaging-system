//
// Created by 86150 on 2026/3/26.
//
#pragma once
#include <QString>

#include "contract/BaseResponse.hpp"

namespace contract::relation
{
    struct AcceptFriendApplicationRequest
    {
        QString applicationId;
        QString recipientRemark; // 同意申请时给对方设置的好友备注
    };

    struct AcceptFriendApplicationResponse : public BaseResponse
    {
    };
}
