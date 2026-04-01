//
// Created by 86150 on 2026/4/1.
//

#pragma once
#include <gtest/gtest.h>

#include "gmock/gmock-nice-strict.h"
#include "message-context/mock/RelationClientMock.h"
#include "sys/message-context/adapter/client/include/BackendClientAdapter.h"
#include "sys/message-context/application/include/MessageApplicationService.h"


class MessageApplicationServiceTestFixture : public testing::Test
{
protected:
    void SetUp()
    {
    }


    void TearDown()
    {
    }


    std::unique_ptr<sys::message::adapter::BackendClientAdapter> backendClient;
    std::unique_ptr<testing::StrictMock<tests::message::mock::RelationClientMock>> mockRelationClient;
    std::unique_ptr<sys::message::application::MessageApplicationService> messageApplicationService;
};
