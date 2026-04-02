#pragma once

#include <gtest/gtest.h>

#include "relation-context/application/fake/PrivateDataBaseFake.h"
#include "relation-context/application/fake/ChatApiGatewayFake.h"
#include "sys/relation-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/relation-context/adapter/repository/include/FriendApplicationRepositoryAdapter.h"
#include "sys/relation-context/adapter/repository/include/FriendRelationRepositoryAdapter.h"
#include "sys/relation-context/adapter/repository/include/ParticipantRepositoryAdapter.h"
#include "sys/relation-context/adapter/repository/include/PrivateChatSessionRepositoryAdapter.h"
#include "sys/relation-context/application/service/include/RelationApplicationService.h"
#include "sys/relation-context/domain/service/include/FriendApplicationService.h"
#include "sys/relation-context/domain/service/include/FriendApplicationValidator.h"
#include "sys/relation-context/domain/service/include/FriendShipService.h"
#include "sys/relation-context/domain/service/include/ParticipantService.h"
#include "sys/relation-context/domain/service/include/PrivateChatSessionService.h"
#include "common/MockUserClient.h"
#include "relation-context/mock/MockMessageClient.h"
#include "relation-context/mock/MockNotificationClient.h"
#include "sys/relation-context/application/assembler/include/FriendApplicationViewAssembler.h"
#include "sys/relation-context/application/assembler/include/ChatSessionViewAssembler.h"

class RelationApplicationServiceTestFixture : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;
    static contract::relation::SendFriendApplicationRequest buildValidSendFriendApplicationRequest();
    static contract::relation::AcceptFriendApplicationRequest buildAcceptRequest(
        const QString& applicationId, const QString& remark);
    static contract::relation::RejectFriendApplicationRequest buildRejectRequest(const QString& applicationId);
    void seedPendingApplication(const QString& friendApplicationId,
                                const QString& applicantUserId,
                                const QString& targetUserId,
                                const QString& verificationMessage = "msg_a",
                                const QString& recipientRemark = "remark_a");
    void seedPendingApplication(const QString& targetUserId);
    void seedAcceptedApplication(const QString& friendApplicationId,
                                 const QString& applicantUserId,
                                 const QString& targetUserId);
    void seedRejectedApplication(const QString& friendApplicationId,
                                 const QString& applicantUserId,
                                 const QString& targetUserId);
    void seedApplication(const QString& friendApplicationId,
                         const QString& applicantUserId,
                         const QString& targetUserId,
                         const QString& verificationMessage,
                         const QString& recipientRemark,
                         const QDateTime& applyTime,
                         const sys::relation::domain::ApplicationStatus status);
    void seedPrivateChatSession(const QString& friendShipId,
                                const QString& privateChatSessionId,
                                const QString& peerUserId,
                                const QString& peerRemark = "我给好友的备注");

protected:
    tests::common::fake::ChatApiGatewayFake chatApiGatewayFake;
    std::unique_ptr<tests::common::fake::PrivateDataBaseFake> privateDataBaseFake;
    std::unique_ptr<sys::relation::adapter::BackendClientAdapter> backendClient;
    std::unique_ptr<sys::relation::adapter::FriendApplicationRepositoryAdapter> friendApplicationRepository;
    std::unique_ptr<sys::relation::adapter::FriendShipRepositoryAdapter> friendShipRepository;
    std::unique_ptr<sys::relation::adapter::PrivateChatSessionRepositoryAdapter> privateChatSessionRepository;
    std::unique_ptr<sys::relation::adapter::ParticipantRepositoryAdapter> participantRepository;
    std::unique_ptr<sys::relation::domain::FriendShipService> friendShipService;
    std::unique_ptr<sys::relation::domain::PrivateChatSessionService> privateChatSessionService;
    std::unique_ptr<sys::relation::domain::ParticipantService> participantService;
    std::unique_ptr<sys::relation::domain::FriendApplicationValidator> validator;
    std::unique_ptr<sys::relation::domain::FriendApplicationService> friendApplicationService;
    std::unique_ptr<sys::relation::application::RelationApplicationService> relationApplicationService;
    std::unique_ptr<testing::StrictMock<tests::common::mock::UserClientMock>> mockUserClient;
    std::unique_ptr<testing::StrictMock<tests::common::mock::MockMessageClient>> mockMessageClient;
    std::unique_ptr<testing::StrictMock<tests::common::mock::MockNotificationClient>> mockNotificationClient;
    std::unique_ptr<sys::relation::application::FriendApplicationViewAssembler> friendApplicationViewAssembler;
    std::unique_ptr<sys::relation::application::ChatSessionViewAssembler> chatSessionViewAssembler;
    QString currentUserId = "100000001";
};
