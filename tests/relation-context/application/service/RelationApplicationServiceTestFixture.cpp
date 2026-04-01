//
// Created by 86150 on 2026/4/1.
//
#include "RelationApplicationServiceTestFixture.h"

void RelationApplicationServiceTestFixture::SetUp()
{
    sys::common::component::UserCredentialManager::instance().update("100000001", "token");

    // 准备Mock端口
    mockMessageClient = std::make_unique<testing::StrictMock<tests::relation::mock::MockMessageClient>>();
    mockNotificationClient = std::make_unique<testing::StrictMock<tests::relation::mock::MockNotificationClient>>();
    mockUserClient = std::make_unique<testing::StrictMock<tests::relation::mock::UserClientMock>>();

    // 准备Fake类
    privateDataBaseFake = std::make_unique<tests::relation::fake::PrivateDataBaseFake>();

    // 准备注入Fake的Adapter
    friendApplicationRepository = std::make_unique<sys::relation::adapter::FriendApplicationRepositoryAdapter>(
        privateDataBaseFake.get());
    friendShipRepository = std::make_unique<sys::relation::adapter::FriendShipRepositoryAdapter>(
        privateDataBaseFake.get());
    privateChatSessionRepository = std::make_unique<sys::relation::adapter::PrivateChatSessionRepositoryAdapter>(
        privateDataBaseFake.get());
    participantRepository = std::make_unique<sys::relation::adapter::ParticipantRepositoryAdapter>(
        privateDataBaseFake.get());
    backendClient = std::make_unique<sys::relation::adapter::BackendClientAdapter>(&chatApiGatewayFake);

    // 准备注入Adapter的领域服务
    validator = std::make_unique<sys::relation::domain::FriendApplicationValidator>(
        friendApplicationRepository.get(),
        friendShipRepository.get());

    friendShipService = std::make_unique<sys::relation::domain::FriendShipService>(friendShipRepository.get());
    privateChatSessionService = std::make_unique<sys::relation::domain::PrivateChatSessionService>(
        privateChatSessionRepository.get(), participantRepository.get());
    participantService = std::make_unique<sys::relation::domain::ParticipantService>(participantRepository.get());
    friendApplicationService = std::make_unique<sys::relation::domain::FriendApplicationService>(
        backendClient.get(),
        validator.get(),
        friendApplicationRepository.get(),
        friendShipRepository.get());

    // 领域服务之间的事件订阅
    friendApplicationService->registerFriendApplicationAcceptedEventSubscriber(friendShipService.get());
    friendApplicationService->registerFriendApplicationAcceptedEventSubscriber(privateChatSessionService.get());
    friendApplicationService->registerFriendApplicationAcceptedEventSubscriber(participantService.get());

    // 准备注入Mock端口或Adapter的Assembler
    friendApplicationViewAssembler = std::make_unique<sys::relation::application::FriendApplicationViewAssembler>(
        mockUserClient.get());

    chatSessionViewAssembler = std::make_unique<sys::relation::application::ChatSessionViewAssembler>(
        mockMessageClient.get(),
        mockNotificationClient.get(),
        participantRepository.get(),
        mockUserClient.get());

    // 最后准备RelationApplicationService
    relationApplicationService = std::make_unique<sys::relation::application::RelationApplicationService>(
        friendApplicationService.get(),
        friendApplicationViewAssembler.get(),
        privateChatSessionService.get(),
        chatSessionViewAssembler.get());
    chatApiGatewayFake.setCurrentUserId("100000001");
    chatApiGatewayFake.seedUsers({"100000001", "100000002", "100000003"});
}

void RelationApplicationServiceTestFixture::TearDown()
{
    sys::common::component::UserCredentialManager::instance().update("", "");
}

contract::relation::SendFriendApplicationRequest
RelationApplicationServiceTestFixture::buildValidSendFriendApplicationRequest()
{
    contract::relation::SendFriendApplicationRequest request;
    request.targetUserId = "100000002";
    request.verificationMessage = "ok_msg";
    request.recipientRemark = "ok_remark";
    return request;
}

contract::relation::AcceptFriendApplicationRequest RelationApplicationServiceTestFixture::buildAcceptRequest(
    const QString& applicationId, const QString& remark)
{
    contract::relation::AcceptFriendApplicationRequest request;
    request.applicationId = applicationId;
    request.recipientRemark = remark;
    return request;
}

contract::relation::RejectFriendApplicationRequest RelationApplicationServiceTestFixture::buildRejectRequest(
    const QString& applicationId)
{
    contract::relation::RejectFriendApplicationRequest request;
    request.friendApplicationId = applicationId;
    return request;
}

void RelationApplicationServiceTestFixture::seedPendingApplication(const QString& friendApplicationId,
                                                                   const QString& applicantUserId,
                                                                   const QString& targetUserId,
                                                                   const QString& verificationMessage,
                                                                   const QString& recipientRemark)
{
    seedApplication(friendApplicationId,
                    applicantUserId,
                    targetUserId,
                    "msg_a",
                    "remark_a",
                    QDateTime::currentDateTime(),
                    sys::relation::domain::ApplicationStatus::Pending);
}

void RelationApplicationServiceTestFixture::seedPendingApplication(const QString& targetUserId)
{
    seedPendingApplication("FA-PENDING", "100000001", targetUserId);
}

void RelationApplicationServiceTestFixture::seedAcceptedApplication(const QString& friendApplicationId,
                                                                    const QString& applicantUserId,
                                                                    const QString& targetUserId)
{
    seedApplication(friendApplicationId,
                    applicantUserId,
                    targetUserId,
                    "msg_a",
                    "remark_a",
                    QDateTime::currentDateTime(),
                    sys::relation::domain::ApplicationStatus::Accepted);
}

void RelationApplicationServiceTestFixture::seedRejectedApplication(const QString& friendApplicationId,
                                                                    const QString& applicantUserId,
                                                                    const QString& targetUserId)
{
    seedApplication(friendApplicationId,
                    applicantUserId,
                    targetUserId,
                    "msg_a",
                    "remark_a",
                    QDateTime::currentDateTime(),
                    sys::relation::domain::ApplicationStatus::Rejected);
}

void RelationApplicationServiceTestFixture::seedApplication(const QString& friendApplicationId,
                                                            const QString& applicantUserId,
                                                            const QString& targetUserId,
                                                            const QString& verificationMessage,
                                                            const QString& recipientRemark,
                                                            const QDateTime& applyTime,
                                                            const sys::relation::domain::ApplicationStatus status)
{
    const auto application = sys::relation::domain::FriendApplication::of(
        applicantUserId,
        friendApplicationId,
        targetUserId,
        verificationMessage,
        recipientRemark,
        applyTime);

    if (status == sys::relation::domain::ApplicationStatus::Accepted)
    {
        application->accept();
    }
    else if (status == sys::relation::domain::ApplicationStatus::Rejected)
    {
        application->reject();
    }

    friendApplicationRepository->save(application);

    tests::relation::fake::ChatApiGatewayFake::FriendApplicationState fakeStatus =
        tests::relation::fake::ChatApiGatewayFake::FriendApplicationState::Pending;
    if (status == sys::relation::domain::ApplicationStatus::Accepted)
    {
        fakeStatus = tests::relation::fake::ChatApiGatewayFake::FriendApplicationState::Accepted;
    }
    else if (status == sys::relation::domain::ApplicationStatus::Rejected)
    {
        fakeStatus = tests::relation::fake::ChatApiGatewayFake::FriendApplicationState::Rejected;
    }

    chatApiGatewayFake.seedFriendApplication(
        friendApplicationId,
        applicantUserId,
        targetUserId,
        fakeStatus);
}


void RelationApplicationServiceTestFixture::seedPrivateChatSession(const QString& friendShipId,
                                                                   const QString& privateChatSessionId,
                                                                   const QString& peerUserId,
                                                                   const QString& peerRemark)
{
    // 创建好友关系
    auto friendShip = sys::relation::domain::FriendShip::of(
        friendShipId,
        "100000001", // 当前用户
        peerUserId);
    friendShipRepository->save(friendShip);

    // 创建私聊会话
    auto privateChatSession = sys::relation::domain::PrivateChatSession::of(
        privateChatSessionId,
        friendShipId);
    privateChatSessionRepository->save(privateChatSession);

    // 创建参与者记录
    auto participant1 = sys::relation::domain::Participant::of(
        "PT-" + privateChatSessionId + "-1",
        privateChatSessionId,
        "100000001",
        sys::relation::domain::ParticipantRole::PrivateMember); // 好友给我的备注我看不到

    auto participant2 = sys::relation::domain::Participant::of(
        "PT-" + privateChatSessionId + "-2",
        privateChatSessionId,
        peerUserId,
        sys::relation::domain::ParticipantRole::PrivateMember, peerRemark);

    // 使用saveAll方法保存参与者
    QVector<QSharedPointer<sys::relation::domain::Participant>> participants;
    participants.append(participant1);
    participants.append(participant2);
    participantRepository->saveAll(participants);
}
