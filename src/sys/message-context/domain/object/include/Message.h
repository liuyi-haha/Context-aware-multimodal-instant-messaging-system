//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <optional>

#include <QByteArray>
#include <QDateTime>
#include <QString>

#include "sys/common/FileSize.hpp"

namespace sys::message::domain
{
    enum class ContentType
    {
        Text,
        Photo,
        Document,
        Speech
    };

    class Duration
    {
    private:
        const int seconds = 0;
    };

    class SendingInfo
    {
    private:
        const QDateTime sendTime;
        const QString sentUserId;
    };

    class Content
    {
    public:
        virtual ~Content() = default;
        virtual ContentType getContentType() = 0;
    };

    class FileContent : public Content
    {
    private:
        const QString fileId;
        const std::optional<QByteArray> content;
    };

    class TextContent final : public Content
    {
    public:
        ContentType getContentType() override;

    private:
        const QString text;
    };

    enum class PhotoContentType
    {
        Png
    };

    class PhotoContent final : public FileContent
    {
    public:
        ContentType getContentType() override;

    private:
        const PhotoContentType type = PhotoContentType::Png;
    };

    enum class DocumentContentType
    {
        Txt,
        Pdf
    };

    class DocumentContent final : public FileContent
    {
    public:
        ContentType getContentType() override;

    private:
        const QString name;
        const DocumentContentType type = DocumentContentType::Txt;
    };

    class SpeechContent final : public FileContent
    {
    public:
        ContentType getContentType() override;

    private:
        const Duration duration;
    };


    class Message
    {
    private:
        QString messageId;
        int seqInChatSession = 0;
        SendingInfo sendingInfo;
        QSharedPointer<Content> content;
    };
}
