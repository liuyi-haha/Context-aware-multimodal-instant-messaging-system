//
// Created by 86150 on 2026/3/28.
//
#include <QSqlDatabase>
#include <QTemporaryFile>

#include "fake/CommonDataBaseFake.h"
#include "fake/UserApiGatewayFake.h"
#include "sys/user-context/adapter/client/include/UserBackendClientAdapter.h"
#include "sys/user-context/adapter/repository/include/UserRepositoryAdapter.h"
#include "sys/user-context/application/service/include/UserApplicationService.h"
#include "sys/user-context/domain/service/include/UserService.h"
#include "sys/user-context/domain/service/include/UserValidator.h"
#include "user-context/mock/MockAuthClient.h"
#include "user-context/mock/MockFileClient.h"

class UserApplicationServiceTestFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // fake
        commonDataBaseFake = std::make_unique<tests::user::fake::CommonDataBaseFake>();
        apiGatewayFake = std::make_unique<tests::user::fake::UserServiceApiGatewayFake>();

        // adapter
        userRepository = std::make_unique<sys::user::adapter::UserRepositoryAdapter>(commonDataBaseFake.get());
        backendClient = std::make_unique<sys::user::adapter::BackendClientAdapter>(apiGatewayFake.get());

        // 领域服务
        userValidator = std::make_unique<sys::user::domain::UserValidator>(&authClient);
        userService = std::make_unique<sys::user::domain::UserService>(
            userValidator.get(),
            backendClient.get(),
            &fileClient,
            userRepository.get()
        );

        // 应用服务
        applicationService = std::make_unique<sys::user::application::UserApplicationService>(userService.get());
    }

    contract::user::RegisterRequest buildValidRegisterUserRequest()
    {
        contract::user::RegisterRequest request;
        request.nickname = "有效昵称";
        request.phone = "13900139000";
        request.password = "abc_123";
        QString avatar = "avatar";
        // 创建一个临时文件来模拟头像文件
        if (tempFile.open())
        {
            tempFile.write(avatar.toUtf8()); // avatarData 是 QByteArray
            tempFile.close();
        }


        request.avatarFileInfo = QFileInfo(tempFile.fileName());
        return request;
    }

    static contract::user::SearchUserRequest buildSearchRequest(const QString& keyword)
    {
        contract::user::SearchUserRequest request;
        request.keyword = keyword;
        return request;
    }

    static contract::user::GetUserRequest buildGetUserRequest(const QString& userId)
    {
        contract::user::GetUserRequest request;
        request.userId = userId;
        return request;
    }

protected:
    testing::StrictMock<tests::user::mock::AuthClientMock> authClient;
    testing::StrictMock<tests::user::mock::FileClientMock> fileClient;

    std::unique_ptr<tests::user::fake::UserServiceApiGatewayFake> apiGatewayFake;
    std::unique_ptr<tests::user::fake::CommonDataBaseFake> commonDataBaseFake;
    std::unique_ptr<sys::user::adapter::BackendClientAdapter> backendClient;
    std::unique_ptr<sys::user::adapter::UserRepositoryAdapter> userRepository;
    std::unique_ptr<sys::user::domain::UserValidator> userValidator;
    std::unique_ptr<sys::user::domain::UserService> userService;
    std::unique_ptr<sys::user::application::UserApplicationService> applicationService;
    QTemporaryFile tempFile;
};


