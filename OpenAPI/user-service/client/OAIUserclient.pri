QT += network

HEADERS += \
# Models
    $${PWD}/OAIUserRegisterUser_200_response.h \
    $${PWD}/OAIUserRegisterUser_200_response_data.h \
    $${PWD}/OAIUserRegisterUser_request.h \
    $${PWD}/OAIUserSearchUser_200_response.h \
    $${PWD}/OAIUserSearchUser_200_response_data.h \
# APIs
    $${PWD}/OAIUserDefaultApi.h \
# Others
    $${PWD}/OAIUserHelpers.h \
    $${PWD}/OAIUserHttpRequest.h \
    $${PWD}/OAIUserObject.h \
    $${PWD}/OAIUserEnum.h \
    $${PWD}/OAIUserHttpFileElement.h \
    $${PWD}/OAIUserServerConfiguration.h \
    $${PWD}/OAIUserServerVariable.h \
    $${PWD}/OAIUserOauth.h

SOURCES += \
# Models
    $${PWD}/OAIUserRegisterUser_200_response.cpp \
    $${PWD}/OAIUserRegisterUser_200_response_data.cpp \
    $${PWD}/OAIUserRegisterUser_request.cpp \
    $${PWD}/OAIUserSearchUser_200_response.cpp \
    $${PWD}/OAIUserSearchUser_200_response_data.cpp \
# APIs
    $${PWD}/OAIUserDefaultApi.cpp \
# Others
    $${PWD}/OAIUserHelpers.cpp \
    $${PWD}/OAIUserHttpRequest.cpp \
    $${PWD}/OAIUserHttpFileElement.cpp \
    $${PWD}/OAIUserOauth.cpp
