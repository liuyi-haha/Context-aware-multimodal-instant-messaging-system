//
// Created by 86150 on 2026/3/31.
//

#pragma once
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFontMetrics>
#include <QPainterPath>
#include <QFont>
#include <QTextDocument>

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
            int maxBubbleWidth = option.rect.width() * MAX_BUBBLE_WIDTH_RATIO;

            // 计算内容区域宽度
            int contentWidth = calculateContentWidth(option.rect.width(), isMe);
            int bubbleWidth = qMin(contentWidth - BUBBLE_PADDING * 2, maxBubbleWidth - BUBBLE_PADDING * 2);

            // 计算内容高度
            int contentHeight = calculateContentHeight(index, bubbleWidth);


            // 计算总高度
            int totalHeight = VERTICAL_MARGIN * 2 + contentHeight;
            if (!isMe)
            {
                totalHeight += HEADER_HEIGHT + ELEMENT_SPACING;
            }
            else
            {
                totalHeight += HEADER_HEIGHT + ELEMENT_SPACING;
            }

            return QSize(option.rect.width(), totalHeight);
        }

        void paint(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override
        {
            if (!index.isValid())
                return;

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setRenderHint(QPainter::TextAntialiasing);

            bool isMe = index.data(MessageListModel::IsMeRole).toBool();
            Layout layout = calculateLayout(option, index, isMe);

            // 绘制头像
            drawAvatar(painter, layout.avatarRect, index);

            // 绘制头部（昵称和时间在同一行）
            drawHeader(painter, layout.headerRect, index, isMe);

            // 绘制气泡背景
            drawBubble(painter, layout.bubbleRect, isMe);

            // 绘制内容
            drawContent(painter, layout.contentRect, index);

            painter->restore();
        }

    private:
        struct Layout
        {
            QRect avatarRect;
            QRect headerRect;
            QRect bubbleRect;
            QRect contentRect;
        };

        Layout calculateLayout(const QStyleOptionViewItem& option,
                               const QModelIndex& index,
                               bool isMe) const
        {
            Layout layout;
            int x = option.rect.x();
            int y = option.rect.y();
            int width = option.rect.width();
            int maxBubbleWidth = width * MAX_BUBBLE_WIDTH_RATIO;

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

            // 计算气泡宽度
            int contentWidth = calculateContentWidth(width, isMe);
            QString content = index.data(MessageListModel::ContentRole).toString();
            int typeValue = index.data(MessageListModel::MessageTypeRole).toInt();
            auto messageType = static_cast<contract::message::MessageView::MessageType>(typeValue);

            int bubbleWidth;
            if (messageType == contract::message::MessageView::MessageType::Text)
            {
                bubbleWidth = calculateTextBubbleWidth(content, maxBubbleWidth);
            }
            else
            {
                bubbleWidth = qMin(contentWidth - BUBBLE_PADDING * 2, maxBubbleWidth - BUBBLE_PADDING * 2);
            }
            bubbleWidth += BUBBLE_PADDING * 2;

            // 气泡位置
            int bubbleX, bubbleY;
            int bubbleHeight = calculateContentHeight(index, bubbleWidth - BUBBLE_PADDING * 2) + BUBBLE_PADDING * 2;

            if (isMe)
            {
                bubbleX = layout.avatarRect.left() - AVATAR_SPACING - bubbleWidth;
                bubbleY = y + VERTICAL_MARGIN + HEADER_HEIGHT + ELEMENT_SPACING;
            }
            else
            {
                bubbleX = layout.avatarRect.right() + AVATAR_SPACING;
                bubbleY = y + VERTICAL_MARGIN + HEADER_HEIGHT + ELEMENT_SPACING;
            }

            layout.bubbleRect = QRect(bubbleX, bubbleY, bubbleWidth, bubbleHeight);
            layout.contentRect = layout.bubbleRect.adjusted(
                BUBBLE_PADDING, BUBBLE_PADDING, -BUBBLE_PADDING, -BUBBLE_PADDING
            );

            // 头部位置（昵称和时间）
            if (isMe)
            {
                layout.headerRect = QRect(
                    bubbleX,
                    y + VERTICAL_MARGIN,
                    bubbleWidth,
                    HEADER_HEIGHT
                );
            }
            else
            {
                layout.headerRect = QRect(
                    bubbleX,
                    y + VERTICAL_MARGIN,
                    bubbleWidth,
                    HEADER_HEIGHT
                );
            }

            return layout;
        }

        int calculateContentWidth(int totalWidth, bool isMe) const
        {
            int contentWidth = totalWidth - HORIZONTAL_MARGIN * 2 - AVATAR_SIZE - AVATAR_SPACING;
            if (isMe)
            {
                contentWidth = totalWidth - HORIZONTAL_MARGIN * 2 - AVATAR_SIZE - AVATAR_SPACING;
            }
            return contentWidth;
        }

        int calculateTextBubbleWidth(const QString& text, int maxWidth) const
        {
            QFont font;
            QFontMetrics fm(font);

            // 按行分割文本
            QStringList lines = text.split('\n');
            int maxLineWidth = 0;

            for (const QString& line : lines)
            {
                int lineWidth = fm.horizontalAdvance(line);
                if (lineWidth > maxLineWidth)
                    maxLineWidth = lineWidth;
            }

            // 气泡宽度 = 最长行宽度 + 内边距
            int bubbleWidth = maxLineWidth + BUBBLE_PADDING * 2;

            // 限制最大宽度
            bubbleWidth = qMin(bubbleWidth, maxWidth);

            // 确保最小宽度
            bubbleWidth = qMax(bubbleWidth, MIN_BUBBLE_WIDTH);

            return bubbleWidth;
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
                    int textHeight = calculateTextHeight(content, contentWidth);
                    return textHeight;
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

            QTextDocument doc;
            doc.setPlainText(text);
            doc.setTextWidth(width);
            return qCeil(doc.size().height());
        }

        int calculateImageHeight(int width) const
        {
            return qMin(MAX_IMAGE_HEIGHT, width);
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

        void drawHeader(QPainter* painter, const QRect& rect, const QModelIndex& index, bool isMe) const
        {
            painter->save();

            QFont font = painter->font();
            font.setPointSize(font.pointSize() - 1);
            painter->setFont(font);

            // 设置浅色字体
            painter->setPen(QColor(150, 150, 150));

            QString nickname = index.data(MessageListModel::SenderNicknameRole).toString();
            QDateTime timestamp = index.data(MessageListModel::TimestampRole).toDateTime();
            QString timeStr = formatTime(timestamp);

            if (isMe)
            {
                // 我的消息：时间在左，昵称在右（或者只显示时间）
                QString headerText = timeStr;
                if (!nickname.isEmpty())
                {
                    headerText = timeStr + "  " + nickname;
                }
                painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, headerText);
            }
            else
            {
                // 对方消息：昵称在左，时间在右
                QString headerText = nickname;
                if (!timeStr.isEmpty())
                {
                    headerText = nickname + "  " + timeStr;
                }
                painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, headerText);
            }

            painter->restore();
        }

        void drawBubble(QPainter* painter, const QRect& rect, bool isMe) const
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            QColor bubbleColor = isMe ? QColor(185, 225, 145) : QColor(255, 255, 255);
            painter->setBrush(bubbleColor);
            painter->setPen(QPen(QColor(220, 220, 220), 1));
            // 绘制气泡，根据左右位置决定圆角
            if (isMe)
            {
                // 我的消息：右上角更圆
                QPainterPath path;
                path.addRoundedRect(rect, BUBBLE_RADIUS, BUBBLE_RADIUS);
                painter->drawPath(path);
            }
            else
            {
                // 对方消息：左上角更圆
                QPainterPath path;
                path.addRoundedRect(rect, BUBBLE_RADIUS, BUBBLE_RADIUS);
                painter->drawPath(path);
            }

            painter->restore();
        }

        void drawContent(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            int typeValue = index.data(MessageListModel::MessageTypeRole).toInt();
            auto messageType = static_cast<contract::message::MessageView::MessageType>(typeValue);

            switch (messageType)
            {
            case contract::message::MessageView::MessageType::Text:
                drawTextContent(painter, rect, index);
                break;
            case contract::message::MessageView::MessageType::Image:
                drawImageContent(painter, rect, index);
                break;
            case contract::message::MessageView::MessageType::File:
                drawFileContent(painter, rect, index);
                break;
            case contract::message::MessageView::MessageType::Audio:
                drawAudioContent(painter, rect, index);
                break;
            }
        }

        void drawTextContent(QPainter* painter, const QRect& rect, const QModelIndex& index) const
        {
            QString content = index.data(MessageListModel::ContentRole).toString();

            painter->save();
            painter->setPen(Qt::black);

            QTextDocument doc;
            doc.setPlainText(content);
            doc.setTextWidth(rect.width());

            painter->translate(rect.topLeft());
            doc.drawContents(painter);
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
        static constexpr int HEADER_HEIGHT = 20;
        static constexpr int ELEMENT_SPACING = 30;
        static constexpr int BUBBLE_RADIUS = 8;
        static constexpr int MAX_IMAGE_HEIGHT = 200;
        static constexpr int FILE_HEIGHT = 60;
        static constexpr int AUDIO_HEIGHT = 50;
        static constexpr int MIN_BUBBLE_WIDTH = 60;
        static constexpr float MAX_BUBBLE_WIDTH_RATIO = 0.6f; // 气泡最大宽度为整个Item宽度的60%
    };
}
