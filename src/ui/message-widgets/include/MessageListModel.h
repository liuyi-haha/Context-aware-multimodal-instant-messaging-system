//
// Created by 86150 on 2026/3/31.
//
#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <qtconcurrentrun.h>

#include "dependencyinjector.h"
#include "contract/system-provider/message-context-provider/MessageView.h"
#include "sys/file-context/application/service/include/FileApplicationService.h"

namespace ui::message_widgets
{
    class MessageListModel : public QAbstractListModel
    {
    public:
        enum Roles
        {
            MsgIdRole = Qt::UserRole + 1,
            MessageTypeRole,
            SenderUserIdRole,
            SenderNicknameRole,
            SenderAvatarFileIdRole,
            SenderAvatarRole,
            ContentRole,
            MediaFileIdRole,
            MediaFileNameRole,
            MediaFileSizeRole,
            MediaDataRole,
            MediaDurationRole,
            MediaImageSizeRole,
            MediaLoadStateRole,
            IsMeRole,
            TimestampRole,
            SequenceRole
        };

        explicit MessageListModel(QObject* parent = nullptr)
            : QAbstractListModel(parent)
        {
        }

        int rowCount(const QModelIndex& parent = QModelIndex()) const override
        {
            return parent.isValid() ? 0 : views.size();
        }

        QVariant data(const QModelIndex& index, int role) const override
        {
            if (!index.isValid() || index.row() >= views.size())
                return QVariant();

            const auto& msg = views.at(index.row());

            switch (role)
            {
            case MsgIdRole: return msg.msgId;
            case MessageTypeRole: return static_cast<int>(msg.messageType);
            case SenderUserIdRole: return msg.senderInfo.userId;
            case SenderNicknameRole: return msg.senderInfo.nickname;
            case SenderAvatarFileIdRole: return msg.senderInfo.avatarFileId;
            case SenderAvatarRole: return
                    msg.senderInfo.avatar.has_value() ? msg.senderInfo.avatar.value() : QVariant();
            case ContentRole: return msg.content.has_value() ? msg.content.value() : QVariant();
            case MediaFileIdRole: return msg.media.has_value() ? msg.media->fileId : QVariant();
            case MediaFileNameRole: return msg.media.has_value() ? msg.media->fileName : QVariant();
            case MediaFileSizeRole: return msg.media.has_value() ? msg.media->fileSize : QVariant();
            case MediaDataRole: return msg.media.has_value() && msg.media->data.has_value()
                                           ? msg.media->data.value()
                                           : QVariant();
            case MediaDurationRole: return msg.media.has_value() && msg.media->duration.has_value()
                                               ? msg.media->duration.value()
                                               : QVariant();
            case MediaImageSizeRole: return msg.media.has_value() && msg.media->imageSize.has_value()
                                                ? msg.media->imageSize.value()
                                                : QVariant();
            case MediaLoadStateRole: return msg.media.has_value()
                                                ? static_cast<int>(msg.media->loadState)
                                                : static_cast<int>(contract::message::MediaInfo::LoadState::NotLoaded);
            case IsMeRole: return msg.isMe;
            case TimestampRole: return msg.timestamp;
            case SequenceRole: return msg.sequence;
            default: return QVariant();
            }
        }

        QHash<int, QByteArray> roleNames() const override
        {
            return {
                {MsgIdRole, "msgId"},
                {MessageTypeRole, "messageType"},
                {SenderUserIdRole, "senderUserId"},
                {SenderNicknameRole, "senderNickname"},
                {SenderAvatarFileIdRole, "senderAvatarFileId"},
                {SenderAvatarRole, "senderAvatar"},
                {ContentRole, "content"},
                {MediaFileIdRole, "mediaFileId"},
                {MediaFileNameRole, "mediaFileName"},
                {MediaFileSizeRole, "mediaFileSize"},
                {MediaDataRole, "mediaData"},
                {MediaDurationRole, "mediaDuration"},
                {MediaImageSizeRole, "mediaImageSize"},
                {MediaLoadStateRole, "mediaLoadState"},
                {IsMeRole, "isMe"},
                {TimestampRole, "timestamp"},
                {SequenceRole, "sequence"}
            };
        }

        void setViews(const QList<contract::message::MessageView>& msgs)
        {
            beginResetModel();
            views = msgs;
            endResetModel();

            // 异步加载头像和媒体数据
            for (const contract::message::MessageView& msg : msgs)
            {
                loadAvatarAsync(msg.msgId, msg.senderInfo.avatarFileId);
                // 如果是图片或语音消息，异步加载媒体数据
                if (msg.messageType == contract::message::MessageView::MessageType::Image ||
                    msg.messageType == contract::message::MessageView::MessageType::Audio)
                {
                    // todo @liuyi 异步加载媒体数据
                }
            }
        }

        // 批量添加消息，自动按 sequence 升序插入
        void addMessages(const QList<contract::message::MessageView>& msgs)
        {
            if (msgs.isEmpty())
                return;

            for (const contract::message::MessageView& msg : msgs)
            {
                // todo @liuyi 异步加载媒体数据
                // 异步加载头像
                loadAvatarAsync(msg.msgId, msg.senderInfo.avatarFileId);

                int insertPos = findInsertPosition(msg.sequence);
                beginInsertRows(QModelIndex(), insertPos, insertPos);
                views.insert(insertPos, msg);
                endInsertRows();
            }
        }

        // 获取指定索引的消息
        contract::message::MessageView getMessage(int index) const
        {
            if (index >= 0 && index < views.size())
                return views.at(index);
            return contract::message::MessageView();
        }

        // 更新发送者头像（空实现，由你完成）
        void updateSenderAvatar(const QString& msgId, const QByteArray& avatarData)
        {
            // 找到msgId
            for (int i = 0; i < views.size(); ++i)
            {
                if (views[i].msgId == msgId)
                {
                    views[i].senderInfo.avatar = avatarData;
                    // 通知视图更新
                    QModelIndex idx = index(i);
                    emit dataChanged(idx, idx, {SenderAvatarRole});
                    break;
                }
            }
        }

        // 更新媒体数据（空实现，由你完成）
        void updateMediaData(int msgId, const QByteArray& mediaData, const QSize& imageSize = QSize())
        {
            // TODO: 实现
        }

        // 更新媒体加载状态（空实现，由你完成）
        void updateMediaLoadState(int msgId, contract::message::MediaInfo::LoadState state)
        {
            // TODO: 实现
        }

    private:
        // 根据 sequence 找到插入位置（升序）
        int findInsertPosition(int sequence) const
        {
            for (int i = 0; i < views.size(); ++i)
            {
                if (views[i].sequence > sequence)
                {
                    return i;
                }
            }
            return views.size();
        }

        void loadAvatarAsync(const QString& msgId, const QString& avatarFileId)
        {
            QtConcurrent::run([avatarFileId, this]()-> QByteArray
            {
                if (fileApplicationService == nullptr)
                {
                    return {};
                }
                return fileApplicationService->getFile(avatarFileId);
            }).then(this, [this, msgId](const QByteArray& data)
            {
                qDebug() << "Avatar loaded for msgId:" << msgId << "data size:" << data.size();
                if (!data.isNull())
                {
                    updateSenderAvatar(msgId, data);
                }
            });
        }

    private:
        QList<contract::message::MessageView> views;
        FileApplicationService* fileApplicationService = QInjection::Inject;
    };
}
