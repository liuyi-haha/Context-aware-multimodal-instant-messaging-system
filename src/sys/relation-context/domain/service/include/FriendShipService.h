//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include <QObject>

#include "dependencyinjector.h"
#include "sys/relation-context/domain/event/FriendApplicationAccepted.h"
#include "sys/relation-context/port/repository/include/FriendShipRepository.h"

namespace sys::relation::domain
{
    class FriendShipService : public QObject, public FriendApplicationAcceptedEventSubscriber
    {
        Q_OBJECT

    public:
        explicit FriendShipService(
            port::FriendShipRepository* friendShipRepository = QInjection::Inject);


        void handle(FriendApplicationAccepted friendApplicationAccepted) override;

    private:
        port::FriendShipRepository* friendShipRepository = nullptr;
        void checkConfig();
    };
}
