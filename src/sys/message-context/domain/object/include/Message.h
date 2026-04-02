//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <optional>

#include <QByteArray>
#include <QDateTime>
#include <QSharedPointer>
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
        const QString senderUserId;

    public:
        SendingInfo(const QDateTime& sendTime, const QString& senderUserId);
        QDateTime sendTimeVal() const;
        QString senderUserIdVal() const;
    };

    class Content
    {
    public:
        virtual ~Content() = default;
        virtual ContentType getContentType() = 0;
    };

    class ContentFactory
    {
    public:
        static QSharedPointer<Content> createTextContent(const QString& text);
        // QSharedPointer<Content> createPhotoContent(const QString& fileId, const std::optional<QByteArray>& content);
        // QSharedPointer<Content> createDocumentContent(const QString& fileId, const std::optional<QByteArray>& content, const QString& name);
        // QSharedPointer<Content> createSpeechContent(const QString& fileId, const std::optional<QByteArray>& content, const Duration& duration);
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
        static bool checkText(const QString& text);
        static QSharedPointer<TextContent> of(const QString& text);

        QString textValue() const;

    private:
        explicit TextContent(const QString& text);

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
        QString chatSessionId;
        int seqInChatSession;
        SendingInfo sendingInfo;
        QSharedPointer<Content> content; // 这里设置为指针是为了支持多态
    public:
        static QSharedPointer<Message> ofTextMessage(const QString& messageId, const QString& chatSessionId,
                                                     int seqInChatSession, const QDateTime& sendTime,
                                                     const QString& senderUserId, const QString& text);
        // static QSharedPointer<Message> ofPhotoMessage();
        // static QSharedPointer<Message> ofDocumentMessage();
        // static QSharedPointer<Message> ofSpeechMessage();

        QString messageIdValue();
        int seqInChatSessionValue();
        QDateTime sendTimeValue();
        QString senderUserIdValue();
        QSharedPointer<Content> contentValue();
        QString chatSessionIdValue();

    private:
        Message(const QString& messageId, const QString& chatSessionId, int seqInChatSession,
                const SendingInfo& sendingInfo,
                const QSharedPointer<Content>& content);
    };
}
