//
// Created by 86150 on 2026/3/29.
//

#include <gtest/gtest.h>

#include "notification-context/fake/ConnectionStatusChangedEventPublisherFake.h"
#include "notification-context/fake/NotificationWebsocketGatewayFake.h"
#include "sys/notification-context/adapter/manager/include/NotificationConnectionManagerAdapter.h"
#include "sys/notification-context/application/service/include/NotificationApplicationService.h"

class NotificationApplicationServiceTest : public testing::Test
{
protected:
    void SetUp() override
    {
        connectionManager = std::make_unique<sys::notification::adapter::NotificationConnectionManager>(
            &websocketGateway
        );
        notificationApplicationService = std::make_unique<
            sys::notification::application::NotificationApplicationService>(
            connectionManager.get(),
            &eventPublisher
        );
    }

protected:
    tests::notification::fake::WebsocketGatewayFake websocketGateway;
    tests::notification::fake::ConnectionStatusChangedEventPublisherFake eventPublisher;

    std::unique_ptr<sys::notification::adapter::NotificationConnectionManager> connectionManager;
    std::unique_ptr<sys::notification::application::NotificationApplicationService> notificationApplicationService;
};

TEST_F(NotificationApplicationServiceTest, 尝试建立连接时_如果连接处于Connected状态_应该不会重复连接)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Connected);

    notificationApplicationService->tryToConnect();

    EXPECT_EQ(websocketGateway.openCallCount, 0);
}

TEST_F(NotificationApplicationServiceTest, 尝试建立连接时_如果连接处于Connected状态_应该不会发布连接状态改变事件)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Connected);

    notificationApplicationService->tryToConnect();

    EXPECT_EQ(eventPublisher.publishCount, 0);
}

TEST_F(NotificationApplicationServiceTest, 尝试建立连接时_如果连接处于Connecting状态_应该不会重复连接)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Connecting);

    notificationApplicationService->tryToConnect();

    EXPECT_EQ(websocketGateway.openCallCount, 0);
}

TEST_F(NotificationApplicationServiceTest, 尝试建立连接时_如果连接处于Connecting状态_应该不会发送连接状态改变事件)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Connecting);

    notificationApplicationService->tryToConnect();

    EXPECT_EQ(eventPublisher.publishCount, 0);
}

TEST_F(NotificationApplicationServiceTest, 尝试建立连接时_如果连接处于Disconnected状态_应该会发起连接)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Disconnected);

    notificationApplicationService->tryToConnect();

    EXPECT_EQ(websocketGateway.openCallCount, 1);
}

TEST_F(NotificationApplicationServiceTest, 尝试建立连接时_如果连接处于Disconnected状态_应该会发送连接状态改变事件)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Disconnected);

    notificationApplicationService->tryToConnect();

    ASSERT_EQ(eventPublisher.publishCount, 1);
    ASSERT_EQ(eventPublisher.events.size(), 1U);
    EXPECT_EQ(eventPublisher.events.front().status, sys::notification::port::ConnectionStatus::Connecting);
}
