cmake_minimum_required(VERSION 4.0)
project(instant_messaging_system)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "F:/software/Qt/6.5.3/mingw_64")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../OpenAPI/user-service/client
        ${CMAKE_CURRENT_BINARY_DIR}/user-service-client-build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../OpenAPI/auth-service/client
        ${CMAKE_CURRENT_BINARY_DIR}/auth-service-client-build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../OpenAPI/chat-service/client
        ${CMAKE_CURRENT_BINARY_DIR}/chat-service-client-build)


find_package(Qt6 COMPONENTS
        Core
        Widgets
        Concurrent
        Test
        Websockets
        Sql
        REQUIRED)

set(IMS_SHARED_EXECUTABLE_SOURCES
        sys/auth-context/adapter/client/cpp/BackendClientAdapter.cpp
        sys/auth-context/adapter/client/include/BackendClientAdapter.h
        sys/auth-context/application/service/cpp/AuthApplicationService.cpp
        sys/auth-context/application/service/include/AuthApplicationService.h
        sys/auth-context/domain/object/cpp/UserCredential.cpp
        sys/auth-context/domain/object/include/UserCredential.h
        sys/auth-context/domain/service/cpp/UserCredentialService.cpp
        sys/auth-context/domain/service/include/UserCredentialService.h
        sys/auth-context/port/Client/cpp/BackendClient.cpp
        sys/auth-context/port/Client/include/BackendClient.h
        sys/auth-context/starter/cpp/AuthContextStarter.cpp
        sys/auth-context/starter/include/AuthContextStarter.h

        sys/common/Avatar.hpp
        sys/common/FileSize.hpp
        sys/common/Identity.hpp
        sys/common/RecipientRemark.hpp
        sys/common/component/UserCredentialManager.h
        sys/common/component/Websocket.h
        common/EventBus.h
        sys/common/event/DomainEvent.h
        sys/common/exception/ApplicationDomainException.h
        sys/common/exception/ApplicationException.h
        sys/common/exception/ApplicationInfraException.h
        sys/common/exception/ApplicationValidationException.h
        sys/common/exception/DomainException.h
        sys/common/exception/DomainValidationException.h

        sys/file-context/application/service/cpp/FileApplicationService.cpp
        sys/file-context/application/service/include/FileApplicationService.h
        sys/file-context/domain/object/cpp/File.cpp
        sys/file-context/domain/object/include/File.h
        sys/file-context/starter/cpp/FileContextStarter.cpp
        sys/file-context/starter/include/FileContextStarter.h

        sys/message-context/domain/object/cpp/Message.cpp
        sys/message-context/domain/object/include/Message.h
        sys/message-context/domain/exception/ChatSessionNotFoundException.h

        sys/notification-context/adapter/manager/cpp/NotificationConnectionManagerAdapter.cpp
        sys/notification-context/adapter/manager/include/NotificationConnectionManagerAdapter.h
        sys/notification-context/adapter/publisher/cpp/ConnectionStatusChangedEventPublisherAdapter.cpp
        sys/notification-context/adapter/publisher/include/ConnectionStatusChangedEventPublisherAdapter.h
        sys/notification-context/application/event/include/ConnectionStatusChanged.h
        sys/notification-context/application/handler/cpp/BackendEventHandler.cpp
        sys/notification-context/application/handler/include/BackendEventHandler.h
        sys/notification-context/application/service/cpp/NotificationApplicationService.cpp
        sys/notification-context/application/service/include/NotificationApplicationService.h
        sys/notification-context/port/manager/include/ConnectionManager.h
        sys/notification-context/port/manager/include/UnreadNotificationManager.h
        sys/notification-context/port/publisher/include/ConnectionStatusChangedEventPublisher.h
        sys/notification-context/remote/subscriber/cpp/BackendEventSubscriber.cpp
        sys/notification-context/remote/subscriber/include/BackendEventSubscriber.h
        sys/notification-context/starter/cpp/NotificationContextStarter.cpp
        sys/notification-context/starter/include/NotificationContextStarter.h

        sys/relation-context/adapter/client/cpp/BackendClientAdapter.cpp
        sys/common/adapter/cpp/UserClientAdapter.cpp
        sys/common/port/UserClient.h
        sys/relation-context/adapter/client/include/BackendClientAdapter.h
        sys/relation-context/adapter/repository/cpp/FriendApplicationRepositoryAdapter.cpp
        sys/relation-context/adapter/repository/cpp/FriendRelationRepositoryAdapter.cpp
        sys/relation-context/adapter/repository/cpp/ParticipantRepositoryAdapter.cpp
        sys/relation-context/adapter/repository/cpp/PrivateChatSessionRepositoryAdapter.cpp
        sys/relation-context/adapter/repository/include/FriendApplicationRepositoryAdapter.h
        sys/relation-context/adapter/repository/include/FriendRelationRepositoryAdapter.h
        sys/relation-context/adapter/repository/include/ParticipantRepositoryAdapter.h
        sys/relation-context/adapter/repository/include/PrivateChatSessionRepositoryAdapter.h
        sys/relation-context/application/assembler/cpp/FriendApplicationViewAssembler.cpp
        sys/relation-context/application/assembler/include/FriendApplicationViewAssembler.h
        sys/relation-context/application/service/cpp/RelationApplicationService.cpp
        sys/relation-context/application/service/include/RelationApplicationService.h
        sys/relation-context/domain/event/FriendApplicationAccepted.h
        sys/relation-context/domain/exception/FriendApplicationAlreadyProcessedException.h
        sys/relation-context/domain/exception/FriendApplicationHandleUnauthorizedException.h
        sys/relation-context/domain/exception/FriendApplicationNotFoundException.h
        sys/relation-context/domain/object/cpp/ChatSession.cpp
        sys/relation-context/domain/object/cpp/FriendApplication.cpp
        sys/relation-context/domain/object/cpp/FriendShip.cpp
        sys/relation-context/domain/object/cpp/GroupChatSession.cpp
        sys/relation-context/domain/object/cpp/Participant.cpp
        sys/relation-context/domain/object/cpp/PrivateChatSession.cpp
        sys/relation-context/domain/object/include/ChatSession.h
        sys/relation-context/domain/object/include/FriendApplication.h
        sys/relation-context/domain/object/include/FriendShip.h
        sys/relation-context/domain/object/include/GroupChatSession.h
        sys/relation-context/domain/object/include/Participant.h
        sys/relation-context/domain/object/include/PrivateChatSession.h
        sys/relation-context/domain/service/cpp/FriendApplicationService.cpp
        sys/relation-context/domain/service/cpp/FriendApplicationValidator.cpp
        sys/relation-context/domain/service/cpp/FriendShipService.cpp
        sys/relation-context/domain/service/cpp/ParticipantService.cpp
        sys/relation-context/domain/service/cpp/PrivateChatSessionService.cpp
        sys/relation-context/domain/service/include/FriendApplicationService.h
        sys/relation-context/domain/service/include/FriendApplicationValidator.h
        sys/relation-context/domain/service/include/FriendShipService.h
        sys/relation-context/domain/service/include/ParticipantService.h
        sys/relation-context/domain/service/include/PrivateChatSessionService.h
        sys/relation-context/port/client/cpp/BackendClient.cpp
        sys/relation-context/port/client/include/BackendClient.h
        sys/relation-context/port/repository/include/FriendApplicationRepository.h
        sys/relation-context/port/repository/include/FriendShipRepository.h
        sys/relation-context/port/repository/include/ParticipantRepository.h
        sys/relation-context/port/repository/include/PrivateChatSessionRepository.h
        sys/relation-context/starter/cpp/RelationContextStarter.cpp
        sys/relation-context/starter/include/RelationContextStarter.h

        sys/starter/cpp/AppStarter.cpp
        sys/starter/include/AppStarter.h

        sys/user-context/adapter/client/cpp/UserAuthClientAdapter.cpp
        sys/user-context/adapter/client/cpp/UserBackendClientAdapter.cpp
        sys/user-context/adapter/client/cpp/UserFileClientAdapter.cpp
        sys/user-context/adapter/client/include/UserAuthClientAdapter.h
        sys/user-context/adapter/client/include/UserBackendClientAdapter.h
        sys/user-context/adapter/client/include/UserFileClientAdapter.h
        sys/user-context/adapter/repository/cpp/UserRepositoryAdapter.cpp
        sys/user-context/adapter/repository/include/UserRepositoryAdapter.h
        sys/user-context/application/service/cpp/UserApplicationService.cpp
        sys/user-context/application/service/include/UserApplicationService.h
        sys/user-context/domain/exception/PhoneHasBeenRegistered.h
        sys/user-context/domain/exception/UserNotFoundException.h
        sys/user-context/domain/object/cpp/User.cpp
        sys/user-context/domain/object/include/User.h
        sys/user-context/domain/service/cpp/UserService.cpp
        sys/user-context/domain/service/cpp/UserValidator.cpp
        sys/user-context/domain/service/include/UserService.h
        sys/user-context/domain/service/include/UserValidator.h
        sys/user-context/port/client/include/AuthClient.h
        sys/user-context/port/client/include/BackendClient.h
        sys/user-context/port/client/include/FileClient.h
        sys/user-context/port/repository/cpp/UserRepository.cpp
        sys/user-context/port/repository/include/UserRepository.h
        sys/user-context/starter/cpp/UserContextStarter.cpp
        sys/user-context/starter/include/UserContextStarter.h

        ../third-lib/QInjection/src/dependencycreator.cpp
        ../third-lib/QInjection/src/dependencycreator.h
        ../third-lib/QInjection/src/dependencyinjector.cpp
        ../third-lib/QInjection/src/dependencyinjector.h
        ../third-lib/QInjection/src/dependencypointer.cpp
        ../third-lib/QInjection/src/dependencypointer.h
        ../third-lib/QInjection/src/dependencypool.cpp
        ../third-lib/QInjection/src/dependencypool.h
        ../third-lib/QInjection/src/dep_global.h
        sys/common/exception/InfraExcception.h
        sys/common/component/Database.h
        ../tests/user-context/application/fake/CommonDataBaseFake.h
        ../tests/user-context/application/fake/UserApiGatewayFake.h
        common/EventBus.h
        sys/relation-context/application/assembler/cpp/ChatSessionViewAssembler.cpp
        sys/relation-context/application/assembler/include/ChatSessionViewAssembler.h
        sys/relation-context/port/client/include/NotificationClient.h
        sys/relation-context/port/client/include/MessageClient.h
        sys/message-context/application/cpp/MessageApplicationService.cpp
        sys/message-context/application/include/MessageApplicationService.h
        sys/relation-context/adapter/client/cpp/MessageClientAdapter.cpp
        sys/relation-context/adapter/client/include/MessageClientAdapter.h
        sys/relation-context/adapter/client/cpp/NotificationClientAdapter.cpp
        sys/relation-context/adapter/client/include/NotificationClientAdapter.h
        contract/system-provider/message-context-provider/MessageView.h
        ui/message-widgets/include/MessageDelegate.h
        ui/message-widgets/include/MessageList.h
        ui/message-widgets/include/MessageInputWidget.h
        ui/message-widgets/include/ChatSessionTitleWidget.h
        sys/common/component/FileUtil.h
        sys/message-context/domain/exception/FriendShipNotFoundException.h
        sys/message-context/domain/exception/NotParticipantException.h
        sys/message-context/domain/exception/InvalidTextException.h
        sys/message-context/domain/service/cpp/MessageService.cpp
        sys/message-context/domain/service/include/MessageService.h
        sys/message-context/domain/service/cpp/SenderValidator.cpp
        sys/message-context/domain/service/include/SenderValidator.h
        sys/message-context/port/client/RelationClient.h
        sys/message-context/port/client/BackendClient.h
        sys/message-context/adapter/client/cpp/BackendClientAdapter.cpp
        sys/message-context/adapter/client/include/BackendClientAdapter.h
        sys/message-context/adapter/client/cpp/RelationClientAdapter.cpp
        sys/message-context/adapter/client/include/RelationClientAdapter.h
        sys/message-context/port/repository/MessageRepository.h
        sys/message-context/adapter/repository/cpp/MessageRepositoryAdapter.cpp
        sys/message-context/adapter/repository/include/MessageRepositoryAdapter.h
        sys/message-context/application/cpp/MessageViewAssembler.cpp
        sys/message-context/application/include/MessageViewAssembler.h
)

set(IMS_SHARED_LINK_LIBS
        Qt::Core
        Qt::Widgets
        Qt::Concurrent
        Qt6::WebSockets
        Qt6::Sql
        QtOpenAPIUserService
        QtOpenAPIAuthService
        QtOpenAPIChatService
)
add_executable(instant_messaging_system
        Main.cpp

        contract/BaseResponse.hpp
        contract/system-provider/auth-context-provider/Login.h
        contract/system-provider/relation-context-provider/AcceptFriendApplication.h
        contract/system-provider/relation-context-provider/ChatSessionView.h
        contract/system-provider/relation-context-provider/FriendApplicationView.hpp
        contract/system-provider/relation-context-provider/RejectFriendApplication.h
        contract/system-provider/relation-context-provider/SendFriendApplication.h
        contract/system-provider/user-context-provider/GetUser.h
        contract/system-provider/user-context-provider/Register.hpp
        contract/system-provider/user-context-provider/SearchUser.h
        contract/system-provider/user-context-provider/UserView.hpp

        resource/resources.qrc

        ui/auth-widgets/cpp/LoginWidget.cpp
        ui/auth-widgets/include/LoginWidget.h
        ui/common/UIEVentBus.cpp
        ui/common/UIEVentBus.h
        ui/main-widgets/cpp/MainWindow.cpp
        ui/main-widgets/cpp/MiddleStack.cpp
        ui/main-widgets/cpp/NavBar.cpp
        ui/main-widgets/cpp/RightStack.cpp
        ui/main-widgets/include/MainWindow.h
        ui/main-widgets/include/MiddleStack.h
        ui/main-widgets/include/NavBar.h
        ui/main-widgets/include/RightStack.h
        ui/relation-widgets/cpp/ChatSessionList.cpp
        ui/relation-widgets/cpp/FriendApplicationDetail.cpp
        ui/relation-widgets/cpp/FriendApplicationList.cpp
        ui/relation-widgets/include/ChatSessionList.h
        ui/relation-widgets/include/FriendApplicationDetail.h
        ui/relation-widgets/include/FriendApplicationList.h
        ui/user-widgets/cpp/RegisterWidget.cpp
        ui/user-widgets/cpp/UserIdDisplayWidget.cpp
        ui/user-widgets/include/RegisterWidget.h
        ui/user-widgets/include/UserIdDisplayWidget.h


        ${IMS_SHARED_EXECUTABLE_SOURCES}
        ui/message-widgets/include/MessageListModel.h
        ui/message-widgets/include/MessagePanel.h
        sys/message-context/starter/Starter.h
)

set(IMS_SHARED_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../third-lib/QInjection/src
)

target_include_directories(instant_messaging_system PRIVATE
        ${IMS_SHARED_INCLUDE_DIRS}
)


target_link_libraries(instant_messaging_system
        ${IMS_SHARED_LINK_LIBS}
)

# for test


include(FetchContent)
enable_testing()

# 声明需要获取 googletest
FetchContent_Declare(
        googletest
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/../third-lib/googletest
)

# 使 CMake 可用 googletest
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES
        AUTOMOC OFF
        AUTOUIC OFF
        AUTORCC OFF
)


# 收集 .cpp 文件
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../tests/*.cpp)
add_executable(run_tests
        ${TEST_SOURCES}
        ${IMS_SHARED_EXECUTABLE_SOURCES}
        ../tests/notification-context/remote/BackendEventSubscriberTest.cpp
        ../tests/relation-context/domain/object/FriendApplicationTest.cpp
        ../tests/relation-context/domain/service/FriendApplicationValidatorTest.cpp
        ../tests/relation-context/domain/service/FriendApplicationServiceTest.cpp
        ../tests/relation-context/application/service/SendFriendApplicationTest.cpp
        ../tests/relation-context/application/service/AcceptFriendApplicationTest.cpp
        ../tests/relation-context/application/service/RejectFriendApplicationTest.cpp
        ../tests/relation-context/application/service/GetFriendApplicationsTest.cpp
        ../tests/relation-context/domain/service/FriendshipServiceTest.cpp
        ../tests/relation-context/domain/service/PrivateChatSessionServiceTest.cpp
        ../tests/relation-context/domain/service/ParticipantServiceTest.cpp
        ../tests/relation-context/application/service/GetChatSessionsTest.cpp
        ../tests/relation-context/domain/object/ParticipantTest.cpp
        ../tests/user-context/application/RegisterUserTest.cpp
        ../tests/user-context/application/SearchUserTest.cpp
        ../tests/user-context/application/GetUserTest.cpp
        ../tests/user-context/application/GetUsersTest.cpp
        ../tests/file-context/application/FileApplicationServiceTest.cpp
        ../tests/message-context/domain/object/MessageTest.cpp
        ../tests/message-context/domain/service/MessageServiceTest.cpp
        ../tests/message-context/mock/MessageRepositoryMock.h
        contract/system-provider/message-context-provider/SendTextMessage.h
        ../tests/message-context/application/GetRecentMessagesTest.cpp
        ../tests/message-context/application/GetMessagesBeforeTest.cpp
        ../tests/message-context/application/GetChatSessionLastMessageInfosTest.cpp
        ../tests/relation-context/application/service/CheckSenderHasPermissionToSendMessageTest.cpp
)


#add_executable(run_tests
#        ${CMAKE_CURRENT_SOURCE_DIR}/../tests/user-context/domain/object/UserTest.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/../tests/user-context/domain/service/UserValidatorTest.cpp
#        ${IMS_SHARED_EXECUTABLE_SOURCES}
#        ../tests/user-context/domain/service/UserServiceTest.cpp
#        ../tests/user-context/application/UserApplicationServiceTestFixture.h
#        ../tests/auth-context/domain/service/UserCredentialServiceTest.cpp
#        ../tests/auth-context/application/AuthApplicationServiceTest.cpp
#        ../tests/auth-context/domain/object/PasswordTest.cpp
#        ../tests/notification-context/application/NotificationApplicationServiceTest.cpp
#)

enable_testing()
add_test(NAME RunAllTests COMMAND run_tests)

set_target_properties(run_tests PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
)

target_link_libraries(run_tests PRIVATE
        ${IMS_SHARED_LINK_LIBS}
        gtest_main
        gmock
)
target_include_directories(run_tests PRIVATE
        ${IMS_SHARED_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../tests
)

gtest_discover_tests(run_tests)