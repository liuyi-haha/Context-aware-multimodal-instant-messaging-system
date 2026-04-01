//
// Created by 86150 on 2026/3/26.
//

#pragma once
#include <QObject>

#include "dependencyinjector.h"
#include "sys/relation-context/domain/event/FriendApplicationAccepted.h"
#include "sys/relation-context/port/repository/include/ParticipantRepository.h"

namespace sys::relation::domain
{
    class ParticipantService : public QObject, public FriendApplicationAcceptedEventSubscriber
    {
        Q_OBJECT

    public:
        explicit ParticipantService(
            port::ParticipantRepository* participantRepository = QInjection::Inject,
            QObject* parent = nullptr);

        void handle(FriendApplicationAccepted event) override;

    private:
        port::ParticipantRepository* participantRepository = nullptr;
        void checkConfig();
    };
}
