//
// Created by 86150 on 2026/3/27.
//

#pragma once
#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"
#include "dependencyinjector.h"
#include "sys/relation-context/domain/object/include/FriendApplication.h"
#include "sys/common/port/UserClient.h"

namespace sys::relation::application
{
    class FriendApplicationViewAssembler : public QObject
    {
        Q_OBJECT

    public:
        explicit FriendApplicationViewAssembler(
            common::port::UserClient* userClient = QInjection::Inject);

        QList<contract::relation::FriendApplicationView> toFriendApplicationViews(
            const QList<QSharedPointer<domain::FriendApplication>>& friendApplications) const;

    private:
        common::port::UserClient* userClient = nullptr;

    private:
        void fillAvatarFileIdAndName(QList<contract::relation::FriendApplicationView>& views,
                                     QSet<QString> userIds) const;
        void checkConfig() const;
    };
}
