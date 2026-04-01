#include "../include/UserAuthClientAdapter.h"

#include <QRegularExpression>

namespace sys::user::adapter
{
    port::AuthResult UserAuthClientAdapter::validateAndHashPassword(const QString& password)
    {
        static const QRegularExpression passwordPattern("^[A-Za-z0-9_]{6,}$");
        const bool valid = passwordPattern.match(password).hasMatch();
        return {valid, QStringLiteral("HASH_") + password};
    }
}


