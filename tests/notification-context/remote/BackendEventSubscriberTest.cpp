//
// Created by 86150 on 2026/3/29.
//
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "notification-context/fake/ConnectionStatusChangedEventPublisherFake.h"
#include "sys/notification-context/adapter/manager/include/NotificationConnectionManagerAdapter.h"
#include "sys/notification-context/application/handler/include/BackendEventHandler.h"
#include "sys/notification-context/remote/subscriber/include/BackendEventSubscriber.h"

class BackendEventSubscriberTest : public testing::Test
{
protected:
    void SetUp() override
    {
        connectionManager = std::make_unique<sys::notification::adapter::NotificationConnectionManager>();
        backendEventHandler = std::make_unique<sys::notification::application::BackendEventHandler>(
            connectionManager.get(),
            &publisher
        );
        backendEventSubscriber = std::make_unique<sys::notification::remote::BackendEventSubscriber>(
            backendEventHandler.get()
        );
    }

protected:
    tests::notification::fake::ConnectionStatusChangedEventPublisherFake publisher;
    std::unique_ptr<sys::notification::adapter::NotificationConnectionManager> connectionManager;
    std::unique_ptr<sys::notification::application::BackendEventHandler> backendEventHandler;
    std::unique_ptr<sys::notification::remote::BackendEventSubscriber> backendEventSubscriber;
};

TEST_F(BackendEventSubscriberTest, 上线时_如果连接处于Connected状态_应该不会发布连接状态改变事件)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Connected);

    backendEventSubscriber->subscribeConnectedEvent();

    EXPECT_EQ(publisher.publishCount, 0);
    EXPECT_EQ(connectionManager->getConnectStatus(), sys::notification::port::ConnectionStatus::Connected);
}

TEST_F(BackendEventSubscriberTest, 上线时_如果连接处于DisConnected状态_应该会发布连接状态改变事件)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Disconnected);

    backendEventSubscriber->subscribeConnectedEvent();

    EXPECT_EQ(publisher.publishCount, 1);
    ASSERT_EQ(publisher.events.size(), 1U);
    EXPECT_EQ(publisher.events.front().status, sys::notification::port::ConnectionStatus::Connected);
    EXPECT_EQ(connectionManager->getConnectStatus(), sys::notification::port::ConnectionStatus::Connected);
}

TEST_F(BackendEventSubscriberTest, 上线时_如果连接处于Connecting状态_应该会发布连接状态改变事件)
{
    connectionManager->setConnectionStatus(sys::notification::port::ConnectionStatus::Connecting);

    backendEventSubscriber->subscribeConnectedEvent();

    EXPECT_EQ(publisher.publishCount, 1);
    ASSERT_EQ(publisher.events.size(), 1U);
    EXPECT_EQ(publisher.events.front().status, sys::notification::port::ConnectionStatus::Connected);
    EXPECT_EQ(connectionManager->getConnectStatus(), sys::notification::port::ConnectionStatus::Connected);
}
