#include "ui/relation-widgets/include/ChatSessionList.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

#include <QtConcurrent/QtConcurrent>
#include "ui/common/UIEVentBus.h"

namespace ui::relation_widgets
{
    ChatSessionListModel::ChatSessionListModel(QObject* parent)
        : QAbstractListModel(parent)
    {
    }

    int ChatSessionListModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
        {
            return 0;
        }

        return views.size();
    }

    QVariant ChatSessionListModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= views.size())
        {
            return {};
        }

        const auto& view = views.at(index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case NameRole:
            return view.name;
        case AvatarRole:
            return view.avatar.value_or(QByteArray());
        case LastMessageSummaryRole:
            return view.lastMessageSummary;
        case LastMessageSendTimeRole:
            return view.lastMessageSendTime;
        case UnreadCountRole:
            return view.unreadCount;
        default:
            return {};
        }
    }

    void ChatSessionListModel::setViews(const QList<contract::relation::ChatSessionView>& sortedChatSessions)
    {
        beginResetModel();
        views = sortedChatSessions;
        endResetModel();

        for (const auto& view : sortedChatSessions)
        {
            loadAvatarAsync(view.chatSessionId, view.avatarFileId);
        }
    }

    contract::relation::ChatSessionView ChatSessionListModel::viewAt(const int row) const
    {
        if (row < 0 || row >= views.size())
        {
            return {};
        }

        return views.at(row);
    }

    void ChatSessionListModel::insertViewByLastMessageSendTimeDesc(const contract::relation::ChatSessionView& view)
    {
        // 查找插入位置（按最后消息发送时间降序）
        int insertPos = 0;
        for (; insertPos < views.size(); ++insertPos)
        {
            if (views[insertPos].lastMessageSendTime < view.lastMessageSendTime)
            {
                break;
            }
        }

        // 如果相同会话ID已存在，则先移除旧的
        for (int i = 0; i < views.size(); ++i)
        {
            if (views[i].chatSessionId == view.chatSessionId)
            {
                beginRemoveRows(QModelIndex(), i, i);
                views.removeAt(i);
                endRemoveRows();
                // 调整插入位置
                if (i < insertPos)
                {
                    insertPos--;
                }
                break;
            }
        }
        // 插入新数据
        beginInsertRows(QModelIndex(), insertPos, insertPos);
        views.insert(insertPos, view);
        endInsertRows();

        // 异步加载头像
        loadAvatarAsync(view.chatSessionId, view.avatarFileId);
    }


    void ChatSessionListModel::loadAvatarAsync(const QString& chatSessionId, const QString& avatarFileId)
    {
        if (avatarFileId.trimmed().isEmpty())
        {
            return;
        }

        QtConcurrent::run([avatarFileId, fileApplicationService = fileApplicationService]()-> QByteArray
        {
            if (fileApplicationService == nullptr)
            {
                return {};
            }
            return fileApplicationService->getFile(avatarFileId);
        }).then(this, [this, chatSessionId](const QByteArray& data)
        {
            if (!data.isNull())
            {
                updateAvatarByChatSessionId(chatSessionId, data);
            }
        });
    }

    void ChatSessionListModel::updateAvatarByChatSessionId(const QString& chatSessionId, const QByteArray& avatar)
    {
        for (int row = 0; row < views.size(); ++row)
        {
            if (views[row].chatSessionId != chatSessionId)
            {
                continue;
            }

            views[row].avatar = avatar;
            const auto index = this->index(row, 0);
            emit dataChanged(index, index, {AvatarRole});
            return;
        }
    }

    ChatSessionModelDelegate::ChatSessionModelDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    void ChatSessionModelDelegate::paint(QPainter* painter,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        const QRect rowRect = option.rect.adjusted(2, 2, -2, -2);
        const auto rects = buildLayoutRects(rowRect);

        drawRowBackground(painter, option, rowRect);
        drawAvatar(painter, index.data(ChatSessionListModel::AvatarRole).toByteArray(), rects.avatarRect);
        drawMainText(painter,
                     index.data(ChatSessionListModel::NameRole).toString(),
                     index.data(ChatSessionListModel::LastMessageSummaryRole).toString(),
                     rects.nameRect,
                     rects.summaryRect);
        drawSendTime(painter,
                     index.data(ChatSessionListModel::LastMessageSendTimeRole).toDateTime(),
                     rects.timeRect);
        drawUnreadBadge(painter,
                        index.data(ChatSessionListModel::UnreadCountRole).toInt(),
                        rects.unreadBadgeRect);

        painter->restore();
    }

    QSize ChatSessionModelDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
    {
        return {0, 78};
    }

    QColor ChatSessionModelDelegate::rowBackground(const QStyleOptionViewItem& option)
    {
        if (option.state & QStyle::State_Selected)
        {
            return QColor(211, 211, 211);
        }

        if (option.state & QStyle::State_MouseOver)
        {
            return QColor(234, 234, 234);
        }

        return QColor(247, 247, 247);
    }

    ChatSessionModelDelegate::LayoutRects ChatSessionModelDelegate::buildLayoutRects(const QRect& rowRect)
    {
        constexpr int leftPadding = 8;
        constexpr int avatarSize = 42;
        constexpr int gap = 8;
        constexpr int rightPadding = 8;

        LayoutRects rects;
        rects.avatarRect = QRect(rowRect.left() + leftPadding,
                                 rowRect.top() + (rowRect.height() - avatarSize) / 2,
                                 avatarSize,
                                 avatarSize);

        const int textLeft = rects.avatarRect.right() + gap;
        const int rightRegionLeft = rowRect.right() - rightPadding - 72;

        rects.nameRect = QRect(textLeft, rowRect.top() + 14, rightRegionLeft - textLeft, 20);
        rects.summaryRect = QRect(textLeft, rowRect.top() + 38, rightRegionLeft - textLeft, 20);
        rects.timeRect = QRect(rightRegionLeft, rowRect.top() + 14, 72, 16);
        rects.unreadBadgeRect = QRect(rowRect.right() - rightPadding - 28, rowRect.top() + 39, 28, 20);

        return rects;
    }

    void ChatSessionModelDelegate::drawRowBackground(QPainter* painter,
                                                     const QStyleOptionViewItem& option,
                                                     const QRect& rowRect)
    {
        const QColor bg = rowBackground(option);
        if (bg.alpha() == 0)
        {
            return;
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(bg);
        painter->drawRoundedRect(rowRect, 6, 6);
    }

    void ChatSessionModelDelegate::drawAvatar(QPainter* painter, const QByteArray& avatar, const QRect& avatarRect)
    {
        QPainterPath clipPath;
        clipPath.addRoundedRect(avatarRect, 8, 8);
        painter->setClipPath(clipPath);

        if (avatar.isNull())
        {
            painter->fillRect(avatarRect, QColor(240, 240, 240));
        }
        else
        {
            QPixmap avatarPixmap;
            avatarPixmap.loadFromData(avatar);
            painter->drawPixmap(avatarRect, avatarPixmap);
        }

        painter->setClipping(false);
    }

    void ChatSessionModelDelegate::drawMainText(QPainter* painter,
                                                const QString& name,
                                                const QString& summary,
                                                const QRect& nameRect,
                                                const QRect& summaryRect)
    {
        QFont nameFont = painter->font();
        nameFont.setFamily(QStringLiteral("Microsoft YaHei"));
        nameFont.setPixelSize(14);
        nameFont.setWeight(QFont::DemiBold);
        painter->setFont(nameFont);
        painter->setPen(QColor(31, 31, 31));
        painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, name);

        QFont messageFont = painter->font();
        messageFont.setFamily(QStringLiteral("Microsoft YaHei"));
        messageFont.setPixelSize(13);
        messageFont.setWeight(QFont::Normal);
        painter->setFont(messageFont);
        painter->setPen(QColor(132, 132, 132));
        painter->drawText(summaryRect, Qt::AlignLeft | Qt::AlignVCenter, summary);
    }

    QString formatTime(const QDateTime& time)
    {
        QDateTime localTime = time.toLocalTime();
        QDateTime now = QDateTime::currentDateTime();

        if (localTime.date() == now.date())
        {
            return localTime.toString("hh:mm");
        }
        else if (localTime.date().addDays(1) == now.date())
        {
            return QString("昨天 %1").arg(localTime.toString("hh:mm"));
        }
        else
        {
            return localTime.toString("MM-dd hh:mm");
        }
    }

    void ChatSessionModelDelegate::drawSendTime(QPainter* painter, const QDateTime& sendTime, const QRect& timeRect)
    {
        QFont timeFont = painter->font();
        timeFont.setFamily(QStringLiteral("Microsoft YaHei"));
        timeFont.setPixelSize(11);
        timeFont.setWeight(QFont::Normal);
        painter->setFont(timeFont);
        painter->setPen(QColor(145, 145, 145));

        const QString text = formatTime(sendTime);
        painter->drawText(timeRect, Qt::AlignRight | Qt::AlignVCenter, text);
    }


    void ChatSessionModelDelegate::drawUnreadBadge(QPainter* painter, const int unreadCount,
                                                   const QRect& unreadBadgeRect)
    {
        if (unreadCount <= 0)
        {
            return;
        }

        const QString badgeText = unreadCount > 99 ? QStringLiteral("99+") : QString::number(unreadCount);

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(242, 71, 63));
        painter->drawRoundedRect(unreadBadgeRect, unreadBadgeRect.height() / 2.0, unreadBadgeRect.height() / 2.0);

        QFont badgeFont = painter->font();
        badgeFont.setFamily(QStringLiteral("Microsoft YaHei"));
        badgeFont.setPixelSize(11);
        badgeFont.setWeight(QFont::Medium);
        painter->setFont(badgeFont);
        painter->setPen(Qt::white);
        painter->drawText(unreadBadgeRect, Qt::AlignCenter, badgeText);
    }

    ChatSessionList::ChatSessionList(QWidget* parent)
        : QWidget(parent),
          listView(new QListView(this)),
          listModel(new ChatSessionListModel(this))
    {
        initLayout();
        initStyle();

        listView->setModel(listModel);
        listView->setItemDelegate(new ChatSessionModelDelegate(listView));
        setupConnections();

        // 异步加载聊天会话列表
        QtConcurrent::run([this]()
        {
            if (relationApplicationService == nullptr)
            {
                return QList<contract::relation::ChatSessionView>();
            }
            return relationApplicationService->getChatSessions();
        }).then(this, [this](const QList<contract::relation::ChatSessionView>& chatSessions)
        {
            listModel->setViews(chatSessions);
        });
    }

    void ChatSessionList::initStyle()
    {
        setObjectName(QStringLiteral("chatSessionList"));
        setStyleSheet(
            "#chatSessionList {"
            " background-color: #f5f5f5;"
            " border: none;"
            "}"
            "QListView {"
            " background-color: transparent;"
            " border: none;"
            " outline: none;"
            "}"
            "QScrollBar:vertical {"
            " background: transparent;"
            " width: 6px;"
            " margin: 4px 2px 4px 0px;"
            "}"
            "QScrollBar::handle:vertical {"
            " background: rgba(0, 0, 0, 45);"
            " border-radius: 3px;"
            " min-height: 20px;"
            "}"
            "QScrollBar::handle:vertical:hover {"
            " background: rgba(0, 0, 0, 70);"
            "}"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,"
            "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
            " background: transparent;"
            " height: 0px;"
            "}"
        );
    }

    void ChatSessionList::initLayout()
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(6, 8, 6, 8);
        layout->setSpacing(4);
        layout->addWidget(listView);

        listView->setSelectionMode(QAbstractItemView::SingleSelection);
        listView->setMouseTracking(true);
        listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setUniformItemSizes(true);
    }

    void ChatSessionList::setupConnections()
    {
        connect(listView, &QListView::clicked, this, [this](const QModelIndex& index)
        {
            emit ui::common::UIEventBus::instance()->chatSessionSelected(listModel->viewAt(index.row()));
        });
        connect(ui::common::UIEventBus::instance(), &ui::common::UIEventBus::chatSessionUpdated, this,
                [this](const QString& chatSessionId)
                {
                    QtConcurrent::run([this, chatSessionId]()-> std::optional<contract::relation::ChatSessionView>
                    {
                        // todo @Liuyi 获取聊天会话
                        return relationApplicationService->getChatSession(chatSessionId);
                    }).then(this, [this, chatSessionId](const std::optional<contract::relation::ChatSessionView>& data)
                    {
                        if (!data.has_value())
                        {
                            qDebug() << "Failed to load chat session data for chatSessionId:" << chatSessionId;
                            return;
                        }
                        auto view = data.value();
                        listModel->insertViewByLastMessageSendTimeDesc(view);
                    });
                });
    }
}

