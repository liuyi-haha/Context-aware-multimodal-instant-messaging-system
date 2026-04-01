//
// Created by 86150 on 2026/3/25.
//
#pragma once
#include <QString>
#include <QDateTime>
#include <QObject>

namespace sys::relation::port
{
    struct SendFriendApplicationResult
    {
        QString friendApplicationId;
        QDateTime applyTime;
    };

    struct NewFriendShipInfo
    {
        QString friendShipId;
        QString privateChatSessionId;
        QString applicantParticipantId;
        QString targetUserParticipantId;
    };

    struct AcceptFriendApplicationResult
    {
        bool isFriendShipCreated;
        std::optional<NewFriendShipInfo> newFriendShipInfo;
    };

    class BackendClient : public QObject
    {
        Q_OBJECT

    public:
        virtual ~BackendClient() = default;
        virtual SendFriendApplicationResult sendFriendApplication(
            const QString& targetUserId,
            const QString& verificationMessage,
            const QString& recipientRemark) = 0;
        virtual AcceptFriendApplicationResult acceptFriendApplication(const QString& friendApplicationId, const QString& remark) = 0;
        virtual void rejectFriendApplication(const QString& friendApplicationId) = 0;
    };
}
