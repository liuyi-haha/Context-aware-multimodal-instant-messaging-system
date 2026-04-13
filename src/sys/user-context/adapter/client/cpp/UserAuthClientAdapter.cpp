#include "../include/UserAuthClientAdapter.h"

#include <QRegularExpression>

namespace sys::user::adapter
{
    port::AuthResult UserAuthClientAdapter::validateAndHashPassword(const QString& password)
    {
        bool isValid = authApplicationService->validatePassword(password);
        if (!isValid)
        {
            return {false, QString()};
        }
        return {true, authApplicationService->getHashedPassword(password)};
    }
}


