//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QString>
#include <stdexcept>

namespace sys::core
{
    class RecipientRemark
    {
    public:
        explicit RecipientRemark(const QString& remark = QString())
            : remark(remark)
        {
            if (!checkRecipientRemark(remark))
            {
                throw std::invalid_argument("invalid recipient remark");
            }
        }

        static bool checkRecipientRemark(const QString& value)
        {
            return value.size() <= 30;
        }

        QString value() const
        {
            return remark;
        }

    private:
        QString remark;
    };
}
