//
// Created by 86150 on 2026/4/2.
//
#include "relation-context/application/service/RelationApplicationServiceTestFixture.h"
#include "sys/message-context/domain/exception/ChatSessionNotFoundException.h"
#include "sys/message-context/domain/exception/FriendShipNotFoundException.h"
#include "sys/message-context/domain/exception/NotParticipantException.h"
#include "sys/relation-context/domain/object/include/Participant.h"
#include "utils/ExceptionTestUtils.h"

TEST_F(RelationApplicationServiceTestFixture, 检查发送者是否有权限发送消息时_如果单聊会话不存在_应该抛出异常)
{
    tests::utils::expectThrowWithMessage<sys::message::domain::ChatSessionNotFoundException>(
        [&]()
        {
            relationApplicationService->checkSenderHasPermissionToSendMessage("private_chat_session_not_exist");
        },
        "聊天会话不存在");
}

TEST_F(RelationApplicationServiceTestFixture, 检查发送者是否有权限发送消息时_如果发送者不在单聊会话中_应该抛出异常)
{
    // 创建好友关系，单聊会话，但不把当前用户加入单聊会话的成员中
    QString friendShipId = "FS-001";
    QString privateChatSessionId = "private_chat_session_id";
    auto friendShip = sys::relation::domain::FriendShip::of(friendShipId, currentUserId, "peer_user_id");
    friendShipRepository->save(friendShip);
    auto session = sys::relation::domain::PrivateChatSession::of(privateChatSessionId, friendShipId);
    privateChatSessionRepository->save(session);

    QVector<QSharedPointer<sys::relation::domain::Participant>> participants;
    participants.append(sys::relation::domain::Participant::of(
        "PT-001",
        privateChatSessionId,
        "peer_user_id",
        sys::relation::domain::ParticipantRole::PrivateMember));
    participantRepository->saveAll(participants);

    // 执行并预期抛出异常
    tests::utils::expectThrowWithMessage<sys::message::domain::NotParticipantException>(
        [&]()
        {
            relationApplicationService->checkSenderHasPermissionToSendMessage(privateChatSessionId);
        },
        "你不在该聊天会话中");
}

TEST_F(RelationApplicationServiceTestFixture, 检查发送者是否有权限发送消息时_如果是向单聊会话发送消息但和对方的好友关系已解除_应该抛出异常)
{
    // 创建单聊会话和成员，但不创建好友关系
    QString privateChatSessionId = "private_chat_session_id_no_friendship";
    auto session = sys::relation::domain::PrivateChatSession::of(privateChatSessionId, "FS-NOT-EXIST");
    privateChatSessionRepository->save(session);

    QVector<QSharedPointer<sys::relation::domain::Participant>> participants;
    participants.append(sys::relation::domain::Participant::of(
        "PT-001",
        privateChatSessionId,
        currentUserId,
        sys::relation::domain::ParticipantRole::PrivateMember));
    participants.append(sys::relation::domain::Participant::of(
        "PT-002",
        privateChatSessionId,
        "peer_user_id",
        sys::relation::domain::ParticipantRole::PrivateMember));
    participantRepository->saveAll(participants);

    tests::utils::expectThrowWithMessage<sys::message::domain::FriendShipNotFoundException>(
        [&]()
        {
            relationApplicationService->checkSenderHasPermissionToSendMessage(privateChatSessionId);
        },
        "你们已经不是好友了");
}
