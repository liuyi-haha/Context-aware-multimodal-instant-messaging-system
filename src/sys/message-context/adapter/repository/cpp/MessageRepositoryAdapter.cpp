//
// Created by 86150 on 2026/4/2.
//

#include "../include/MessageRepositoryAdapter.h"

namespace sys::message::adapter
{
    void MessageRepositoryAdapter::save(const QSharedPointer<domain::Message>)
    {
    }

    QSharedPointer<domain::Message> MessageRepositoryAdapter::of(const QString& messageId)
    {
    }
}
