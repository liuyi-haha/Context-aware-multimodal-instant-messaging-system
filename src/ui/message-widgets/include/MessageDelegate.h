//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFontMetrics>
#include <QPainterPath>
#include <QFont>

#include "MessageListModel.h"

namespace ui::message_widgets
{
    class MessageDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        explicit MessageDelegate(QObject* parent = nullptr)
            : QStyledItemDelegate(parent)
        {
        }

        QSize sizeHint(const QStyleOptionViewItem& option,
                       const QModelIndex& index) const override
        {
            if (!index.isValid())
                return QSize(0, 0);

            bool isMe = index.data(MessageListModel::IsMeRole).toBool();
            int width = option.rect.width();

            // 计算内容区域宽度
            int contentWidth = width - HORIZONTAL_MARGIN * 2;
            if (!isMe)
            {
                contentWidth -= (AVATAR_SIZE + AVATAR_SPACING);
            }

            // 计算内容高度
            int contentHeight = calculateContentHeight(index, contentWidth);

            // 计算总高度
            int totalHeight = contentHeight + VERTICAL_MARGIN * 2;
            if (!isMe)
            {
                totalHeight += NICKNAME_HEIGHT + ELEMENT_SPACING;
            }

            return QSize(width, totalHeight);
        }

        void paint(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override
        {
            if (!index.isValid())
                return;

            painter->save();

            // 绘制选中/悬停背景
            if (option.state & QStyle::State_Selected)
            {
                painter->fillRect(option.rect, option.palette.highlight());
            }
            else if (option.state & QStyle::State_MouseOver)
            {
                painter->fillRect(option.rect, QColor(240, 240, 240));
            }

            bool isMe = index.data(MessageListModel::IsMeRole).toBool();
            int contentWidth = option.rect.width() - HORIZONTAL_MARGIN * 2;
            if (!isMe)
            {
                contentWidth -= (AVATAR_SIZE + AVATAR_SPACING);
            }

            int contentHeight = calculateContentHeight(index, contentWidth);
            Layout layout = calculateLayout(option, index, contentHeight);

            // 绘制头像
            drawAvatar(painter, layout.avatarRect, index);

            // 绘制昵称（仅对方）
            if (!isMe)
            {
                drawNickname(painter, layout.nicknameRect, index);
            }

            // 绘制气泡背景
            drawBubble(painter, layout.bubbleRect, isMe);

            // 绘制内容
            int typeValue = index.data(MessageListModel::MessageTypeRole).toInt();
            auto messageType = static_cast<contract::message::MessageView::MessageType>(typeValue);

            switch (messageType)
            {
            case contract::message::MessageView::MessageType::Text:
                drawTextContent(painter, layout.contentRect, index);
                break;
            case contract::message::MessageView::MessageType::Image:
                drawImageContent(painter, layout.contentRect, index);
                break;
            case contract::message::MessageView::MessageType::File:
                drawFileContent(painter, layout.contentRect, index);
                break;
            case contract::message::MessageView::MessageType::Audio:
                drawAudioContent(painter, layout.contentRect, index);
                break;
            }

            // 绘制时间
            drawTime(painter, layout.timeRect, index, isMe);

            painter->restore();
        }

    private:
        struct Layout
        {
            QRect avatarRect;
            QRect nicknameRect;
            QRect bubbleRect;
            QRect contentRect;
            QRect timeRect;
        };

        Layout calculateLayout(const QStyleOptionViewItem& option,
                               const QModelIndex& index,
                               int contentHeight) const
        {
            Layout layout;
            bool isMe = index.data(MessageListModel::IsMeRole).toBool();
            int x = option.rect.x();
            int y = option.rect.y();
            int width = option.rect.width();

            // 头像位置
            if (isMe)
            {
                layout.avatarRect = QRect(
                    x + width - HORIZONTAL_MARGIN - AVATAR_SIZE,
                    y + VERTICAL_MARGIN,
                    AVATAR_SIZE,
                    AVATAR_SIZE
                );
            }
            else
            {
                layout.avatarRect = QRect(
                    x + HORIZONTAL_MARGIN,
                    y + VERTICAL_MARGIN,
                    AVATAR_SIZE,
                    AVATAR_SIZE
                );
            }

            // 气泡位置
            int bubbleX, bubbleY;
            int bubbleWidth = width - HORIZONTAL_MARGIN * 2 - AVATAR_SIZE - AVATAR_SPACING;
            int bubbleHeight = contentHeight;

            if (isMe)
            {
                bubbleX = x + HORIZONTAL_MARGIN;
                bubbleY = y + VERTICAL_MARGIN;
            }
            else
            {
                bubbleX = x + HORIZONTAL_MARGIN + AVATAR_SIZE + AVATAR_SPACING;
                bubbleY = y + VERTICAL_MARGIN + NICKNAME_HEIGHT + ELEMENT_SPACING;
            }

            layout.bubbleRect = QRect(bubbleX, bubbleY, bubbleWidth, bubbleHeight);
            layout.contentRect = layout.bubbleRect.adjusted(
                BUBBLE_PADDING, BUBBLE_PADDING, -BUBBLE_PADDING, -BUBBLE_PADDING
            );

            // 昵称位置（仅对方）
            if (!isMe)
            {
                layout.nicknameRect = QRect(
                    bubbleX,
                    y + VERTICAL_MARGIN,
                    bubbleWidth,
                    NICKNAME_HEIGHT
                );
            }

            // 时间位置
            int timeY = layout.bubbleRect.bottom() + ELEMENT_SPACING;
            layout.timeRect = QRect(bubbleX, timeY, bubbleWidth, TIME_HEIGHT);

            return layout;
        }

        int calculateContentHeight(const QModelIndex& index, int contentWidth) const
        {
            int typeValue = index.data(MessageListModel::MessageTypeRole).toInt();
            auto messageType = static_cast<contract::message::MessageView::MessageType>(typeValue);

            switch (messageType)
            {
            case contract::message::MessageView::MessageType::Text:
                {
                    QString content = index.data(MessageListModel::ContentRole).toString();
                    int textWidth = contentWidth - BUBBLE_PADDING * 2;
                    int textHeight = calculateTextHeight(content, textWidth);
                    return textHeight + BUBBLE_PADDING * 2;
                }
            case contract::message::MessageView::MessageType::Image:
                return calculateImageHeight(contentWidth);
            case contract::message::MessageView::MessageType::File:
                return FILE_HEIGHT;
            case contract::message::MessageView::MessageType::Audio:
                return AUDIO_HEIGHT;
            }
            return 0;
        }

        int calculateTextHeight(const QString& text, int width) const
        {
            if (text.isEmpty())
                return 0;

            QFont font;
            QFontMetrics fm(font);
            QRect rect = fm.boundingRect(
                QRect(0, 0, width, 0),
                Qt::TextWordWrap,
                text);
            return rect.height();
        }

        int calculateImageHeight(int width) const
        {
            return qMin(MAX_IMAGE_HEIGHT, width * 3 / 4);
        }

        void drawAvatar(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            QByteArray avatarData = index.data(MessageListModel::SenderAvatarRole).toByteArray();

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            // 圆形裁剪
            QPainterPath path;
            path.addEllipse(rect);
            painter->setClipPath(path);

            if (!avatarData.isEmpty())
            {
                QPixmap avatar;
                avatar.loadFromData(avatarData);
                painter->drawPixmap(rect, avatar.scaled(rect.size(), Qt::KeepAspectRatioByExpanding,
                                                        Qt::SmoothTransformation));
            }
            else
            {
                // 默认灰色圆形
                painter->setBrush(QColor(200, 200, 200));
                painter->setPen(Qt::NoPen);
                painter->drawEllipse(rect);
                painter->setPen(Qt::gray);
                painter->drawText(rect, Qt::AlignCenter, "?");
            }
            painter->restore();
        }

        void drawNickname(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            QString nickname = index.data(MessageListModel::SenderNicknameRole).toString();

            painter->save();
            QFont font = painter->font();
            font.setBold(true);
            font.setPointSize(font.pointSize() - 1);
            painter->setFont(font);
            painter->setPen(QColor(100, 100, 100));
            painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, nickname);
            painter->restore();
        }

        void drawBubble(QPainter* painter, const QRect& rect, bool isMe) const
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            QColor bubbleColor = isMe ? QColor(220, 248, 198) : QColor(255, 255, 255);
            painter->setBrush(bubbleColor);
            painter->setPen(QPen(QColor(220, 220, 220), 1));
            painter->drawRoundedRect(rect, BUBBLE_RADIUS, BUBBLE_RADIUS);

            painter->restore();
        }

        void drawTextContent(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            QString content = index.data(MessageListModel::ContentRole).toString();

            painter->save();
            painter->setPen(Qt::black);
            painter->drawText(rect, Qt::TextWordWrap, content);
            painter->restore();
        }

        void drawImageContent(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            int loadState = index.data(MessageListModel::MediaLoadStateRole).toInt();
            QByteArray imageData = index.data(MessageListModel::MediaDataRole).toByteArray();

            painter->save();

            if (loadState == static_cast<int>(contract::message::MediaInfo::LoadState::Loaded) && !imageData.isEmpty())
            {
                QPixmap pixmap;
                pixmap.loadFromData(imageData);
                painter->drawPixmap(rect, pixmap.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            else if (loadState == static_cast<int>(contract::message::MediaInfo::LoadState::Loading))
            {
                painter->fillRect(rect, QColor(240, 240, 240));
                painter->setPen(Qt::gray);
                painter->drawText(rect, Qt::AlignCenter, "加载中...");
            }
            else
            {
                painter->fillRect(rect, QColor(240, 240, 240));
                painter->setPen(Qt::gray);
                painter->drawText(rect, Qt::AlignCenter, "点击加载");
            }

            painter->restore();
        }

        void drawFileContent(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            QString fileName = index.data(MessageListModel::MediaFileNameRole).toString();
            qint64 fileSize = index.data(MessageListModel::MediaFileSizeRole).toLongLong();

            painter->save();

            // 文件图标
            QRect iconRect = rect;
            iconRect.setWidth(40);
            painter->fillRect(iconRect, QColor(200, 200, 200));
            painter->setPen(Qt::black);
            painter->drawText(iconRect, Qt::AlignCenter, "📄");

            // 文件名和大小
            QRect textRect = rect;
            textRect.setLeft(iconRect.right() + 8);

            QFont font = painter->font();
            painter->setPen(Qt::black);
            painter->drawText(textRect, Qt::AlignTop, fileName);

            QRect sizeRect = textRect;
            sizeRect.setTop(textRect.top() + 20);
            font.setPointSize(font.pointSize() - 1);
            painter->setFont(font);
            painter->setPen(Qt::gray);
            painter->drawText(sizeRect, Qt::AlignTop, formatFileSize(fileSize));

            painter->restore();
        }

        void drawAudioContent(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            int duration = index.data(MessageListModel::MediaDurationRole).toInt();
            int loadState = index.data(MessageListModel::MediaLoadStateRole).toInt();

            painter->save();

            // 语音图标
            QRect iconRect = rect;
            iconRect.setWidth(30);
            painter->fillRect(iconRect, QColor(100, 150, 200));
            painter->setPen(Qt::white);
            painter->drawText(iconRect, Qt::AlignCenter, "🎤");

            // 时长
            QRect textRect = rect;
            textRect.setLeft(iconRect.right() + 8);
            painter->setPen(Qt::black);

            if (loadState == static_cast<int>(contract::message::MediaInfo::LoadState::Loaded))
            {
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                                  QString("%1秒").arg(duration));
            }
            else if (loadState == static_cast<int>(contract::message::MediaInfo::LoadState::Loading))
            {
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, "加载中...");
            }
            else
            {
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, "点击播放");
            }

            painter->restore();
        }

        void drawTime(QPainter* painter, const QRect& rect, const QModelIndex& index, bool isMe) const
        {
            QDateTime timestamp = index.data(MessageListModel::TimestampRole).toDateTime();

            painter->save();
            QFont font = painter->font();
            font.setPointSize(font.pointSize() - 1);
            painter->setFont(font);
            painter->setPen(QColor(150, 150, 150));

            QString timeStr = formatTime(timestamp);
            if (isMe)
            {
                painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, timeStr);
            }
            else
            {
                painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, timeStr);
            }
            painter->restore();
        }

        QString formatFileSize(qint64 size) const
        {
            if (size < 1024)
                return QString("%1 B").arg(size);
            if (size < 1024 * 1024)
                return QString("%1 KB").arg(size / 1024.0, 0, 'f', 1);
            return QString("%1 MB").arg(size / (1024.0 * 1024.0), 0, 'f', 1);
        }

        QString formatTime(const QDateTime& time) const
        {
            QDateTime now = QDateTime::currentDateTime();

            if (time.date() == now.date())
            {
                return time.toString("hh:mm");
            }
            else if (time.date().addDays(1) == now.date())
            {
                return QString("昨天 %1").arg(time.toString("hh:mm"));
            }
            else
            {
                return time.toString("MM-dd hh:mm");
            }
        }

        // 常量
        static constexpr int HORIZONTAL_MARGIN = 12;
        static constexpr int VERTICAL_MARGIN = 8;
        static constexpr int AVATAR_SIZE = 40;
        static constexpr int AVATAR_SPACING = 8;
        static constexpr int BUBBLE_PADDING = 10;
        static constexpr int NICKNAME_HEIGHT = 20;
        static constexpr int TIME_HEIGHT = 16;
        static constexpr int ELEMENT_SPACING = 4;
        static constexpr int BUBBLE_RADIUS = 8;
        static constexpr int MAX_IMAGE_HEIGHT = 200;
        static constexpr int FILE_HEIGHT = 60;
        static constexpr int AUDIO_HEIGHT = 50;
    };
}
