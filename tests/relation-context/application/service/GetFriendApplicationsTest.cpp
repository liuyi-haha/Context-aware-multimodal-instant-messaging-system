#include "relation-context/application/service/RelationApplicationServiceTestFixture.h"

//------------------------------- 获取所有好友申请 -------------------------------
// rule:
// 好友申请列表应该按照申请时间降序排列
// Pending状态的好友申请，如果是自己发送的，状态栏应该展示为“等待对方处理”，如果是别人发送的，应该展示为“待处理”
// 好友申请的姓名栏，如果是自己发送的好友申请，应该展示为申请时填写的对方备注，如果是别人发送的好友申请，应该展示为对方昵称
// 好友申请的头像栏应该展示对方头像


TEST_F(RelationApplicationServiceTestFixture, 获取所有好友申请时_单条记录应该完整映射FriendApplicationView的所有字段)
{
    EXPECT_CALL(*mockUserClient, getUsers(testing::_))
        .Times(1)
        .WillOnce(testing::Return(QHash<QString, sys::relation::port::UserInfo>{
                                                          {
                                                              "100000002",
                                                              sys::relation::port::UserInfo{
                                                                  "100000002", "MappedNick", "mapped-avatar"
                                                              }
                                                          }
                                                      }));
    EXPECT_CALL(*mockUserClient, getUser(testing::_)).Times(0);


    const QDateTime applyTime(QDate(2026, 3, 30), QTime(16, 20, 30), Qt::UTC);
    seedApplication("FA-FULL-FIELDS", "100000001", "100000002", "hello_verify", "my_remark", applyTime,
                    sys::relation::domain::ApplicationStatus::Pending);

    const auto views = relationApplicationService->getFriendApplications();

    ASSERT_EQ(views.size(), 1);
    const auto& view = views[0];

    EXPECT_EQ(view.applicationId, "FA-FULL-FIELDS");
    EXPECT_EQ(view.peerUserId, "100000002");
    EXPECT_EQ(view.peerUserAvatarFileId, "mapped-avatar");
    EXPECT_FALSE(view.peerUserAvatar.has_value());
    EXPECT_EQ(view.name, "my_remark");
    EXPECT_EQ(view.verificationMessage, "hello_verify");
    EXPECT_EQ(view.status, contract::relation::FriendApplicationView::Status::WaitingForVerification);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(view.status), "等待验证");
    EXPECT_EQ(view.applyTime, applyTime);
}

TEST_F(RelationApplicationServiceTestFixture, 获取所有好友申请时_如果只有自己发送的好友申请时_应该能按照申请时间降序显示)
{
    EXPECT_CALL(*mockUserClient, getUsers(testing::_))
        .Times(1)
        .WillOnce(testing::Return(QHash<QString, sys::relation::port::UserInfo>{
                                                          {
                                                              "100000002",
                                                              sys::relation::port::UserInfo{
                                                                  "100000002", "Nick-2", "avatar-2"
                                                              }
                                                          },
                                                          {
                                                              "100000003",
                                                              sys::relation::port::UserInfo{
                                                                  "100000003", "Nick-3", "avatar-3"
                                                              }
                                                          }
                                                      }));
    EXPECT_CALL(*mockUserClient, getUser(testing::_)).Times(0);


    const QDateTime oldTime(QDate(2026, 3, 30), QTime(10, 0, 0), Qt::UTC);
    const QDateTime newTime(QDate(2026, 3, 30), QTime(11, 0, 0), Qt::UTC);

    seedApplication("FA-SELF-OLD", "100000001", "100000002", "msg_old", "remark_old", oldTime,
                    sys::relation::domain::ApplicationStatus::Pending);
    seedApplication("FA-SELF-NEW", "100000001", "100000003", "msg_new", "remark_new", newTime,
                    sys::relation::domain::ApplicationStatus::Pending);

    const auto views = relationApplicationService->getFriendApplications();

    ASSERT_EQ(views.size(), 2);
    EXPECT_EQ(views[0].applicationId, "FA-SELF-NEW");
    EXPECT_EQ(views[1].applicationId, "FA-SELF-OLD");

    EXPECT_EQ(views[0].status, contract::relation::FriendApplicationView::Status::WaitingForVerification);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[0].status), "等待验证");
    EXPECT_EQ(views[1].status, contract::relation::FriendApplicationView::Status::WaitingForVerification);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[1].status), "等待验证");

    EXPECT_EQ(views[0].name, "remark_new");
    EXPECT_EQ(views[1].name, "remark_old");
    EXPECT_EQ(views[0].peerUserAvatarFileId, "avatar-3");
    EXPECT_EQ(views[1].peerUserAvatarFileId, "avatar-2");
}

TEST_F(RelationApplicationServiceTestFixture, 获取所有好友申请时_如果只有别人发送的好友申请_应该能按照申请时间降序显示)
{
    EXPECT_CALL(*mockUserClient, getUsers(testing::_))
        .Times(1)
        .WillOnce(testing::Return(QHash<QString, sys::relation::port::UserInfo>{
                                                          {
                                                              "100000002",
                                                              sys::relation::port::UserInfo{
                                                                  "100000002", "Friend-A", "avatar-a"
                                                              }
                                                          },
                                                          {
                                                              "100000003",
                                                              sys::relation::port::UserInfo{
                                                                  "100000003", "Friend-B", "avatar-b"
                                                              }
                                                          }
                                                      }));
    EXPECT_CALL(*mockUserClient, getUser(testing::_)).Times(0);

    const QDateTime oldTime(QDate(2026, 3, 30), QTime(9, 0, 0), Qt::UTC);
    const QDateTime newTime(QDate(2026, 3, 30), QTime(12, 0, 0), Qt::UTC);

    seedApplication("FA-OTHER-OLD", "100000002", "100000001", "msg_a", "remark_a", oldTime,
                    sys::relation::domain::ApplicationStatus::Pending);
    seedApplication("FA-OTHER-NEW", "100000003", "100000001", "msg_b", "remark_b", newTime,
                    sys::relation::domain::ApplicationStatus::Pending);

    const auto views = relationApplicationService->getFriendApplications();

    ASSERT_EQ(views.size(), 2);
    EXPECT_EQ(views[0].applicationId, "FA-OTHER-NEW");
    EXPECT_EQ(views[1].applicationId, "FA-OTHER-OLD");

    EXPECT_EQ(views[0].status, contract::relation::FriendApplicationView::Status::Pending);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[0].status), "待处理");
    EXPECT_EQ(views[1].status, contract::relation::FriendApplicationView::Status::Pending);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[1].status), "待处理");

    EXPECT_EQ(views[0].name, "Friend-B");
    EXPECT_EQ(views[1].name, "Friend-A");
    EXPECT_EQ(views[0].peerUserAvatarFileId, "avatar-b");
    EXPECT_EQ(views[1].peerUserAvatarFileId, "avatar-a");
}

TEST_F(RelationApplicationServiceTestFixture, 获取所有好友申请时_如果同时有自己发送的和别人发送的好友申请_应该能按照申请时间降序显示_姓名栏和状态栏应该区别显示)
{
    EXPECT_CALL(*mockUserClient, getUsers(testing::_))
        .Times(1)
        .WillOnce(testing::Return(QHash<QString, sys::relation::port::UserInfo>{
                                                          {
                                                              "100000002",
                                                              sys::relation::port::UserInfo{
                                                                  "100000002", "Nick-2", "avatar-2"
                                                              }
                                                          },
                                                          {
                                                              "100000003",
                                                              sys::relation::port::UserInfo{
                                                                  "100000003", "Nick-3", "avatar-3"
                                                              }
                                                          },
                                                          {
                                                              "100000004",
                                                              sys::relation::port::UserInfo{
                                                                  "100000004", "Nick-4", "avatar-4"
                                                              }
                                                          }
                                                      }));
    EXPECT_CALL(*mockUserClient, getUser(testing::_)).Times(0);


    seedApplication("FA-MIX-5", "100000001", "100000002", "v5", "self_pending",
                    QDateTime(QDate(2026, 3, 30), QTime(15, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Pending);
    seedApplication("FA-MIX-4", "100000003", "100000001", "vfour", "other_pending",
                    QDateTime(QDate(2026, 3, 30), QTime(14, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Pending);
    seedApplication("FA-MIX-3", "100000001", "100000004", "v3", "self_accepted",
                    QDateTime(QDate(2026, 3, 30), QTime(13, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Accepted);
    seedApplication("FA-MIX-2", "100000002", "100000001", "v2", "other_rejected",
                    QDateTime(QDate(2026, 3, 30), QTime(12, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Rejected);
    seedApplication("FA-MIX-1", "100000004", "100000001", "v1", "other_accepted",
                    QDateTime(QDate(2026, 3, 30), QTime(11, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Accepted);

    const auto views = relationApplicationService->getFriendApplications();

    ASSERT_EQ(views.size(), 5);
    EXPECT_EQ(views[0].applicationId, "FA-MIX-5");
    EXPECT_EQ(views[1].applicationId, "FA-MIX-4");
    EXPECT_EQ(views[2].applicationId, "FA-MIX-3");
    EXPECT_EQ(views[3].applicationId, "FA-MIX-2");
    EXPECT_EQ(views[4].applicationId, "FA-MIX-1");

    EXPECT_EQ(views[0].status, contract::relation::FriendApplicationView::Status::WaitingForVerification);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[0].status), "等待验证");
    EXPECT_EQ(views[1].status, contract::relation::FriendApplicationView::Status::Pending);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[1].status), "待处理");
    EXPECT_EQ(views[2].status, contract::relation::FriendApplicationView::Status::Accepted);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[2].status), "已同意");
    EXPECT_EQ(views[3].status, contract::relation::FriendApplicationView::Status::Rejected);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[3].status), "已拒绝");
    EXPECT_EQ(views[4].status, contract::relation::FriendApplicationView::Status::Accepted);
    EXPECT_EQ(contract::relation::FriendApplicationView::statusToText(views[4].status), "已同意");

    EXPECT_EQ(views[0].name, "self_pending");
    EXPECT_EQ(views[1].name, "Nick-3");
    EXPECT_EQ(views[2].name, "self_accepted");
    EXPECT_EQ(views[3].name, "Nick-2");
    EXPECT_EQ(views[4].name, "Nick-4");

    EXPECT_EQ(views[0].peerUserAvatarFileId, "avatar-2");
    EXPECT_EQ(views[1].peerUserAvatarFileId, "avatar-3");
    EXPECT_EQ(views[2].peerUserAvatarFileId, "avatar-4");
    EXPECT_EQ(views[3].peerUserAvatarFileId, "avatar-2");
    EXPECT_EQ(views[4].peerUserAvatarFileId, "avatar-4");
}


