//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <gtest/gtest.h>

#include "message-context/application/fake/ChatApiGatewayFake.h"
#include "gmock/gmock-nice-strict.h"
#include "message-context/mock/RelationClientMock.h"
#include "relation-context/application/fake/PrivateDataBaseFake.h"
#include "sys/message-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/message-context/adapter/repository/include/MessageRepositoryAdapter.h"
#include "sys/message-context/application/include/MessageApplicationService.h"
#include "sys/message-context/domain/service/include/MessageService.h"
#include "sys/message-context/domain/service/include/SenderValidator.h"


class MessageApplicationServiceTestFixture : public testing::Test
{
protected:
    void SetUp() override
    {
        // 设置用户信息
        sys::common::component::UserCredentialManager::instance().update("123456789", "");
        // 准备Mock端口
        mockRelationClient = std::make_unique<testing::StrictMock<tests::message::mock::RelationClientMock>>();
        // 准备fake
        fakeChatApiGateway = std::make_unique<testing::StrictMock<tests::message::fake::ChatApiGatewayFake>>();
        fakePrivateDataBase = std::make_unique<testing::StrictMock<tests::relation::fake::PrivateDataBaseFake>>();
        // 准备注入fake的Adapter
        backendClientAdapter = std::make_unique<sys::message::adapter::BackendClientAdapter>(fakeChatApiGateway.get());
        messageRepositoryAdapter = std::make_unique<sys::message::adapter::MessageRepositoryAdapter>(
            fakePrivateDataBase.get());
        // 准备领域服务
        senderValidator = std::make_unique<sys::message::domain::SenderValidator>(mockRelationClient.get());
        messageService = std::make_unique<sys::message::domain::MessageService>(
            backendClientAdapter.get(), senderValidator.get(), messageRepositoryAdapter.get()
        );
        // 准备装配器
        messageViewAssembler = std::make_unique<sys::message::application::MessageViewAssembler>();
        // 准备应用服务
        applicationService = std::make_unique<sys::message::application::MessageApplicationService>(
            messageViewAssembler.get(), messageService.get());
    }


    void TearDown() override
    {
        sys::common::component::UserCredentialManager::instance().update("", "");
    }

    std::unique_ptr<testing::StrictMock<tests::message::mock::RelationClientMock>> mockRelationClient;
    std::unique_ptr<testing::StrictMock<tests::message::fake::ChatApiGatewayFake>> fakeChatApiGateway;
    std::unique_ptr<tests::relation::fake::PrivateDataBaseFake> fakePrivateDataBase;

    // 仓储
    std::unique_ptr<sys::message::adapter::MessageRepositoryAdapter> messageRepositoryAdapter;
    // 客户端
    std::unique_ptr<sys::message::adapter::BackendClientAdapter> backendClientAdapter;
    // 领域服务
    std::unique_ptr<sys::message::domain::SenderValidator> senderValidator;
    std::unique_ptr<sys::message::domain::MessageService> messageService;
    // 装配器
    std::unique_ptr<sys::message::application::MessageViewAssembler> messageViewAssembler;
    // 应用服务
    std::unique_ptr<sys::message::application::MessageApplicationService> applicationService;
};
