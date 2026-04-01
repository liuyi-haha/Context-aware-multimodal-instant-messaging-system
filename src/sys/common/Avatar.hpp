//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <optional>
#include <QByteArray>
#include <QString>

namespace sys::core
{
    class Avatar
    {
    public:
        explicit Avatar(const QString& avatarFileId)
            : avtarFileId(avatarFileId)
        {
        }

        Avatar(const QString& avtar_file_id, const std::optional<QByteArray>& avatar)
            : avtarFileId(avtar_file_id),
              avatar(avatar)
        {
        }

        QString avatarFileId() const
        {
            return avtarFileId;
        }

        std::optional<QByteArray> avatarData() const
        {
            return avatar;
        }

    private:
        const QString avtarFileId;
        const std::optional<QByteArray> avatar;
    };
}
