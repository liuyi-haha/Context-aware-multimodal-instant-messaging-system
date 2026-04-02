#pragma once

#include <optional>

#include <QHash>
#include <QSet>

#include "sys/relation-context/adapter/client/include/BackendClientAdapter.h"

namespace tests::common::fake
{
    class ChatApiGatewayFake : public sys::relation::adapter::IChatApiGateway
    {
    public:
        enum class FriendApplicationState
        {
            Pending,
            Accepted,
            Rejected
        };

        struct FakeFriendApplication
        {
            QString id;
            QString applicantUserId;
            QString targetUserId;
            FriendApplicationState state = FriendApplicationState::Pending;
        };

        struct AcceptOutcome
        {
            bool createFriendShip = false;
            QString friendShipId;
            QString privateChatSessionId;
            QString applicantParticipantId;
            QString targetUserParticipantId;
        };

        void setCurrentUserId(const QString& userId)
        {
            currentUserId = userId.trimmed();
            if (!currentUserId.isEmpty())
            {
                knownUsers.insert(currentUserId);
            }
        }

        void seedUser(const QString& userId)
        {
            const QString normalized = userId.trimmed();
            if (!normalized.isEmpty())
            {
                knownUsers.insert(normalized);
            }
        }

        void seedUsers(std::initializer_list<QString> userIds)
        {
            for (const auto& userId : userIds)
            {
                seedUser(userId);
            }
        }

        void seedFriendShip(const QString& id1, const QString& id2)
        {
            friendshipKeys.insert(buildFriendKey(id1, id2));
        }

        void seedFriendApplication(const QString& id,
                                   const QString& applicantUserId,
                                   const QString& targetUserId,
                                   const FriendApplicationState state)
        {
            FakeFriendApplication application;
            application.id = id;
            application.applicantUserId = applicantUserId.trimmed();
            application.targetUserId = targetUserId.trimmed();
            application.state = state;
            friendApplications.insert(application.id, application);
            seedUsers({application.applicantUserId, application.targetUserId});
        }

        [[nodiscard]] std::optional<QString> lastGeneratedFriendApplicationId() const
        {
            return lastGeneratedSendFriendApplicationId;
        }

        [[nodiscard]] std::optional<AcceptOutcome> lastAcceptOutcome() const
        {
            return lastAcceptResult;
        }

        OpenAPIChat::OAIChatSendFriendApplication_200_response sendFriendApplication(
            const OpenAPIChat::OAIChatSendFriendApplication_request& request) override
        {
            ++sendCallCount;
            lastRequest = request;

            const QString targetUserId = request.getTargetUserId().trimmed();
            if (!knownUsers.contains(targetUserId))
            {
                return buildSendError("USER_NOT_FOUND", "用户不存在");
            }
            if (targetUserId == currentUserId)
            {
                return buildSendError("SELF_FRIEND", "不能添加自己为好友");
            }
            if (friendshipKeys.contains(buildFriendKey(currentUserId, targetUserId)))
            {
                return buildSendError("ALREADY_FRIEND", "双方已经是好友");
            }
            if (hasPendingApplication(currentUserId, targetUserId))
            {
                return buildSendError("DUPLICATE_APPLICATION", "已存在待处理的好友申请");
            }

            const QString friendApplicationId = nextId("FA", nextFriendApplicationSeq);
            seedFriendApplication(friendApplicationId, currentUserId, targetUserId, FriendApplicationState::Pending);
            lastGeneratedSendFriendApplicationId = friendApplicationId;

            OpenAPIChat::OAIChatSendFriendApplication_200_response_data data;
            data.setFriendApplicationId(friendApplicationId);

            OpenAPIChat::OAIChatSendFriendApplication_200_response response;
            response.setSuccess(true);
            response.setData(data);
            response.setErrCode(QString());
            response.setErrMsg(QString());
            return response;
        }

        OpenAPIChat::OAIChatAcceptFriendApplication_200_response acceptFriendApplication(
            const OpenAPIChat::OAIChatAcceptFriendApplication_request& request,
            const QString& friendApplicationId) override
        {
            ++acceptCallCount;
            lastAcceptFriendApplicationId = friendApplicationId;

            if (!friendApplications.contains(friendApplicationId))
            {
                return buildAcceptError("APPLICATION_NOT_FOUND", "好友申请不存在");
            }

            auto application = friendApplications.value(friendApplicationId);
            if (application.state != FriendApplicationState::Pending)
            {
                return buildAcceptError("APPLICATION_ALREADY_HANDLED", "好友申请已被处理");
            }

            application.state = FriendApplicationState::Accepted;
            friendApplications.insert(friendApplicationId, application);

            OpenAPIChat::OAIChatAcceptFriendApplication_200_response_data data;
            const QString key = buildFriendKey(application.applicantUserId, application.targetUserId);
            const bool createFriendShip = !friendshipKeys.contains(key);
            data.setCreateFriendShip(createFriendShip);

            AcceptOutcome outcome;
            outcome.createFriendShip = createFriendShip;

            if (createFriendShip)
            {
                friendshipKeys.insert(key);
                outcome.friendShipId = nextId("FS", nextFriendShipSeq);
                outcome.privateChatSessionId = nextId("PCS", nextPrivateChatSessionSeq);
                outcome.applicantParticipantId = nextId("PT", nextParticipantSeq);
                outcome.targetUserParticipantId = nextId("PT", nextParticipantSeq);

                data.setFriendshipId(outcome.friendShipId);
                data.setPrivateChatSessionId(outcome.privateChatSessionId);
                data.setApplicantParticipantId(outcome.applicantParticipantId);
                data.setTargetUserParticipantId(outcome.targetUserParticipantId);
            }
            lastAcceptResult = outcome;

            OpenAPIChat::OAIChatAcceptFriendApplication_200_response response;
            response.setSuccess(true);
            response.setData(data);
            response.setErrCode(QString());
            response.setErrMsg(QString());
            return response;
        }

        OpenAPIChat::OAIChatRejectFriendApplication_200_response rejectFriendApplication(
            const QString& friendApplicationId) override
        {
            ++rejectCallCount;
            lastRejectFriendApplicationId = friendApplicationId;

            if (!friendApplications.contains(friendApplicationId))
            {
                return buildRejectError("APPLICATION_NOT_FOUND", "好友申请不存在");
            }

            auto application = friendApplications.value(friendApplicationId);
            if (application.state != FriendApplicationState::Pending)
            {
                return buildRejectError("APPLICATION_ALREADY_HANDLED", "好友申请已被处理");
            }

            application.state = FriendApplicationState::Rejected;
            friendApplications.insert(friendApplicationId, application);

            OpenAPIChat::OAIChatRejectFriendApplication_200_response response;
            response.setSuccess(true);
            response.setErrCode(QString());
            response.setErrMsg(QString());
            return response;
        }

    private:
        static QString buildFriendKey(const QString& id1, const QString& id2)
        {
            QString left = id1.trimmed().toUpper();
            QString right = id2.trimmed().toUpper();
            if (left > right)
            {
                left.swap(right);
            }
            return left + QStringLiteral("|") + right;
        }

        static QString nextId(const QString& prefix, int& seq)
        {
            const QString id = prefix + QString("%1").arg(seq, 9, 10, QChar('0'));
            ++seq;
            return id;
        }

        bool hasPendingApplication(const QString& applicantUserId, const QString& targetUserId) const
        {
            for (const auto& application : friendApplications)
            {
                if (application.applicantUserId == applicantUserId
                    && application.targetUserId == targetUserId
                    && application.state == FriendApplicationState::Pending)
                {
                    return true;
                }
            }
            return false;
        }

        static OpenAPIChat::OAIChatSendFriendApplication_200_response buildSendError(
            const QString& errCode,
            const QString& errMsg)
        {
            OpenAPIChat::OAIChatSendFriendApplication_200_response response;
            response.setSuccess(false);
            response.setErrCode(errCode);
            response.setErrMsg(errMsg);
            return response;
        }

        static OpenAPIChat::OAIChatAcceptFriendApplication_200_response buildAcceptError(
            const QString& errCode,
            const QString& errMsg)
        {
            OpenAPIChat::OAIChatAcceptFriendApplication_200_response response;
            response.setSuccess(false);
            response.setErrCode(errCode);
            response.setErrMsg(errMsg);
            return response;
        }

        static OpenAPIChat::OAIChatRejectFriendApplication_200_response buildRejectError(
            const QString& errCode,
            const QString& errMsg)
        {
            OpenAPIChat::OAIChatRejectFriendApplication_200_response response;
            response.setSuccess(false);
            response.setErrCode(errCode);
            response.setErrMsg(errMsg);
            return response;
        }

    public:
        int sendCallCount = 0;
        int acceptCallCount = 0;
        int rejectCallCount = 0;
        QString lastAcceptFriendApplicationId;
        QString lastRejectFriendApplicationId;
        OpenAPIChat::OAIChatSendFriendApplication_request lastRequest;

    private:
        QString currentUserId = QStringLiteral("100000001");
        QSet<QString> knownUsers{QStringLiteral("100000001")};
        QSet<QString> friendshipKeys;
        QHash<QString, FakeFriendApplication> friendApplications;
        std::optional<QString> lastGeneratedSendFriendApplicationId;
        std::optional<AcceptOutcome> lastAcceptResult;
        int nextFriendApplicationSeq = 900000001;
        int nextFriendShipSeq = 900000001;
        int nextPrivateChatSessionSeq = 900000001;
        int nextParticipantSeq = 900000001;
    };
}
