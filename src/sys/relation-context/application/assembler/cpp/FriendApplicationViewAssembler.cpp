//
// Created by 86150 on 2026/3/27.
//

#include "../include/FriendApplicationViewAssembler.h"

#include <QSet>

#include "sys/common/component/UserCredentialManager.h"

namespace
{
    contract::relation::FriendApplicationView::Status toViewStatus(
        const sys::relation::domain::ApplicationStatus status,
        const bool isCurrentUserApplicant)
    {
        switch (status)
        {
        case sys::relation::domain::ApplicationStatus::Accepted:
            return contract::relation::FriendApplicationView::Status::Accepted;
        case sys::relation::domain::ApplicationStatus::Rejected:
            return contract::relation::FriendApplicationView::Status::Rejected;
        case sys::relation::domain::ApplicationStatus::Pending:
            return isCurrentUserApplicant
                       ? contract::relation::FriendApplicationView::Status::WaitingForVerification
                       : contract::relation::FriendApplicationView::Status::Pending;
        default:
            return contract::relation::FriendApplicationView::Status::Pending;
        }
    }
}

namespace sys::relation::application
{
    FriendApplicationViewAssembler::FriendApplicationViewAssembler(
        port::UserClient* userClient)
        : userClient(userClient)
    {
    }

    void FriendApplicationViewAssembler::fillAvatarFileIdAndName(
        QList<contract::relation::FriendApplicationView>& views, QSet<QString> userIds) const
    {
        const QHash<QString, port::UserInfo> users = userClient->getUsers(userIds);
        for (auto& view : views)
        {
            const auto userIt = users.constFind(view.peerUserId);
            if (userIt == users.cend())
            {
                continue;
            }

            const auto& userInfo = userIt.value();
            const QString nickname = userInfo.nickname;
            const QString avatarFileId = userInfo.avatarFileId;


            view.peerUserAvatarFileId = avatarFileId;
            if (view.name.isEmpty())
            {
                view.name = nickname;
            }
        }
    }

    QList<contract::relation::FriendApplicationView> FriendApplicationViewAssembler::toFriendApplicationViews(
        const QList<QSharedPointer<domain::FriendApplication>>& friendApplications) const
    {
        checkConfig();

        QList<contract::relation::FriendApplicationView> views;
        views.reserve(friendApplications.size());

        const QString currentUserId = common::component::UserCredentialManager::instance().getCurrentUserId();
        QSet<QString> shouldFillNameApplicationIds;
        QSet<QString> userIds;

        for (const auto& friendApplication : friendApplications)
        {
            if (friendApplication == nullptr)
            {
                continue;
            }
            contract::relation::FriendApplicationView view;
            const bool isCurrentUserApplicant = friendApplication->applicantUserId() == currentUserId;

            view.applicationId = friendApplication->friendApplicationId();

            view.peerUserId = (isCurrentUserApplicant
                               ? friendApplication->targetUserId()
                               : friendApplication->applicantUserId());
            view.peerUserAvatarFileId = ""; // 后面会统一加载用户信息时设置
            view.peerUserAvatar = std::nullopt;
            view.name = isCurrentUserApplicant
                            ? friendApplication->recipientRemarkValue()
                            : ""; // 后面会统一加载用户信息时设置
            view.verificationMessage = friendApplication->verificationMessageValue();
            view.status = toViewStatus(friendApplication->applicationStatus(), isCurrentUserApplicant);
            view.applyTime = friendApplication->applyTime();

            userIds.insert(view.peerUserId);
            views.push_back(view);
        }


        fillAvatarFileIdAndName(views, userIds);

        return views;
    }

    void FriendApplicationViewAssembler::checkConfig() const
    {
        if (userClient == nullptr)
        {
            throw std::runtime_error("FriendApplicationViewAssembler dependency is not properly configured.");
        }
    }
}
