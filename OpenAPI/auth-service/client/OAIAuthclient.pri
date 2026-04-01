QT += network

HEADERS += \
# Models
    $${PWD}/OAIAuthLogin_200_response.h \
    $${PWD}/OAIAuthLogin_200_response_data.h \
    $${PWD}/OAIAuthLogin_request.h \
# APIs
    $${PWD}/OAIAuthDefaultApi.h \
# Others
    $${PWD}/OAIAuthHelpers.h \
    $${PWD}/OAIAuthHttpRequest.h \
    $${PWD}/OAIAuthObject.h \
    $${PWD}/OAIAuthEnum.h \
    $${PWD}/OAIAuthHttpFileElement.h \
    $${PWD}/OAIAuthServerConfiguration.h \
    $${PWD}/OAIAuthServerVariable.h \
    $${PWD}/OAIAuthOauth.h

SOURCES += \
# Models
    $${PWD}/OAIAuthLogin_200_response.cpp \
    $${PWD}/OAIAuthLogin_200_response_data.cpp \
    $${PWD}/OAIAuthLogin_request.cpp \
# APIs
    $${PWD}/OAIAuthDefaultApi.cpp \
# Others
    $${PWD}/OAIAuthHelpers.cpp \
    $${PWD}/OAIAuthHttpRequest.cpp \
    $${PWD}/OAIAuthHttpFileElement.cpp \
    $${PWD}/OAIAuthOauth.cpp
