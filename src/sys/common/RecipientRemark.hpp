//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>
#include <stdexcept>

#include "sys/relation-context/domain/exception/InvalidRecipientRemarkException.h"

namespace sys::core
{
    class RecipientRemark
    {
    public:
        explicit RecipientRemark(const QString& remark = QString(), bool validate = true)
            : remark(remark)
        {
            if (validate && !checkRecipientRemark(remark))
            {
                throw relation::domain::InvalidRecipientRemarkException();
            }
        }

        static bool checkRecipientRemark(const QString& value)
        {
            return value.size() <= 20 && value.size() >= 1;
        }

        QString value() const
        {
            return remark;
        }

    private:
        QString remark;
    };
}
