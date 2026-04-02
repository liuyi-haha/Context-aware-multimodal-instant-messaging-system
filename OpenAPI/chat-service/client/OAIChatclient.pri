QT += network

HEADERS += \
# Models
    $${PWD}/OAIChatAcceptFriendApplication_200_response.h \
    $${PWD}/OAIChatAcceptFriendApplication_200_response_data.h \
    $${PWD}/OAIChatAcceptFriendApplication_request.h \
    $${PWD}/OAIChatRejectFriendApplication_200_response.h \
    $${PWD}/OAIChatSendFriendApplication_200_response.h \
    $${PWD}/OAIChatSendFriendApplication_200_response_data.h \
    $${PWD}/OAIChatSendFriendApplication_request.h \
    $${PWD}/OAIChatSendTextMessage_200_response.h \
    $${PWD}/OAIChatSendTextMessage_200_response_data.h \
    $${PWD}/OAIChatSendTextMessage_request.h \
# APIs
    $${PWD}/OAIChatDefaultApi.h \
# Others
    $${PWD}/OAIChatHelpers.h \
    $${PWD}/OAIChatHttpRequest.h \
    $${PWD}/OAIChatObject.h \
    $${PWD}/OAIChatEnum.h \
    $${PWD}/OAIChatHttpFileElement.h \
    $${PWD}/OAIChatServerConfiguration.h \
    $${PWD}/OAIChatServerVariable.h \
    $${PWD}/OAIChatOauth.h

SOURCES += \
# Models
    $${PWD}/OAIChatAcceptFriendApplication_200_response.cpp \
    $${PWD}/OAIChatAcceptFriendApplication_200_response_data.cpp \
    $${PWD}/OAIChatAcceptFriendApplication_request.cpp \
    $${PWD}/OAIChatRejectFriendApplication_200_response.cpp \
    $${PWD}/OAIChatSendFriendApplication_200_response.cpp \
    $${PWD}/OAIChatSendFriendApplication_200_response_data.cpp \
    $${PWD}/OAIChatSendFriendApplication_request.cpp \
    $${PWD}/OAIChatSendTextMessage_200_response.cpp \
    $${PWD}/OAIChatSendTextMessage_200_response_data.cpp \
    $${PWD}/OAIChatSendTextMessage_request.cpp \
# APIs
    $${PWD}/OAIChatDefaultApi.cpp \
# Others
    $${PWD}/OAIChatHelpers.cpp \
    $${PWD}/OAIChatHttpRequest.cpp \
    $${PWD}/OAIChatHttpFileElement.cpp \
    $${PWD}/OAIChatOauth.cpp
