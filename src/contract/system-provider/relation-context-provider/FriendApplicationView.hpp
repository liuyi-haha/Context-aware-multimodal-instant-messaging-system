#pragma once
#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <optional>

namespace contract::relation
{
    struct FriendApplicationView
    {
        enum class Status
        {
            Accepted,
            Rejected,

            // 申请人为对方，且未被处理时
            Pending,
            // 申请人为当前用户好友，且未被处理时
            WaitingForVerification
        };

        static QString statusToText(const Status status)
        {
            switch (status)
            {
            case Status::Pending:
                return QStringLiteral("待处理");
            case Status::Accepted:
                return QStringLiteral("已同意");
            case Status::Rejected:
                return QStringLiteral("已拒绝");
            case Status::WaitingForVerification:
                return QStringLiteral("等待验证");
            default:
                return QStringLiteral("未知状态");
            }
        }

        FriendApplicationView() = default;

        QString applicationId;
        QString peerUserId; //不管是当前用户是不是申请人，都是对方的用户ID
        QString peerUserAvatarFileId; // 不管是当前用户是不是申请人，都是对方的用户头像文件ID
        std::optional<QByteArray> peerUserAvatar;
        QString name; // 如果当前用户是申请人，则是对方备注，否则是对方昵称
        QString verificationMessage;
        Status status;
        QDateTime applyTime;
    };
}
