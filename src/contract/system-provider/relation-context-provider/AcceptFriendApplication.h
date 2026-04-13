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
        // todo @liuyi 添加单聊会话信息
        bool isNewFriendShip; // 如果为false，则下面的字段无效
        QString privateChatSessionId;
    };
}
