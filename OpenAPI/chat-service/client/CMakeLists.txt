cmake_minimum_required(VERSION 3.5)

project(QtOpenAPIChatService)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Gui)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME}
  OAIChatAcceptFriendApplication_200_response.h
  OAIChatAcceptFriendApplication_200_response_data.h
  OAIChatAcceptFriendApplication_request.h
  OAIChatRejectFriendApplication_200_response.h
  OAIChatSendFriendApplication_200_response.h
  OAIChatSendFriendApplication_200_response_data.h
  OAIChatSendFriendApplication_request.h
  OAIChatSendTextMessage_200_response.h
  OAIChatSendTextMessage_200_response_data.h
  OAIChatSendTextMessage_request.h
  OAIChatDefaultApi.h
  OAIChatHelpers.h
  OAIChatHttpRequest.h
  OAIChatObject.h
  OAIChatEnum.h
  OAIChatHttpFileElement.h
  OAIChatServerConfiguration.h
  OAIChatServerVariable.h
  OAIChatOauth.h
  OAIChatAcceptFriendApplication_200_response.cpp
  OAIChatAcceptFriendApplication_200_response_data.cpp
  OAIChatAcceptFriendApplication_request.cpp
  OAIChatRejectFriendApplication_200_response.cpp
  OAIChatSendFriendApplication_200_response.cpp
  OAIChatSendFriendApplication_200_response_data.cpp
  OAIChatSendFriendApplication_request.cpp
  OAIChatSendTextMessage_200_response.cpp
  OAIChatSendTextMessage_200_response_data.cpp
  OAIChatSendTextMessage_request.cpp
  OAIChatDefaultApi.cpp
  OAIChatHelpers.cpp
  OAIChatHttpRequest.cpp
  OAIChatHttpFileElement.cpp
  OAIChatOauth.cpp
  )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Gui

)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)

install(
    FILES ${HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
