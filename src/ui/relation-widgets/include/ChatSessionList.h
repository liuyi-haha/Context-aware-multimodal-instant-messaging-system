#pragma once
#include <QAbstractListModel>
#include <QList>
#include <QListView>
#include <QStyledItemDelegate>
#include <QStyleOption>
#include <QWidget>

#include "contract/system-provider/relation-context-provider/ChatSessionView.h"
#include "dependencyinjector.h"
#include "sys/file-context/application/service/include/FileApplicationService.h"
#include "sys/relation-context/application/service/include/RelationApplicationService.h"

namespace ui::relation_widgets
{
    class ChatSessionListModel : public QAbstractListModel
    {
    public:
        enum Roles
        {
            TypeRole = Qt::UserRole + 1,
            NameRole,
            AvatarRole,
            LastMessageSummaryRole,
            LastMessageSendTimeRole,
            UnreadCountRole
        };

        explicit ChatSessionListModel(QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;

        void setViews(const QList<contract::relation::ChatSessionView>& sortedChatSessions);
        contract::relation::ChatSessionView viewAt(int row) const;
        void insertViewByLastMessageSendTimeDesc(const contract::relation::ChatSessionView& view);

    private:
        QList<contract::relation::ChatSessionView> views;
        sys::relation::application::RelationApplicationService* relationApplicationService = QInjection::Inject;
        FileApplicationService* fileApplicationService = QInjection::Inject;

    private:
        void loadAvatarAsync(const QString& chatSessionId, const QString& avatarFileId);
        void updateAvatarByChatSessionId(const QString& chatSessionId, const QByteArray& avatar);
    };


    class ChatSessionModelDelegate : public QStyledItemDelegate
    {
    public:
        explicit ChatSessionModelDelegate(QObject* parent = nullptr);

        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;

    private:
        struct LayoutRects
        {
            QRect avatarRect;
            QRect nameRect;
            QRect summaryRect;
            QRect timeRect;
            QRect unreadBadgeRect;
        };

        static QColor rowBackground(const QStyleOptionViewItem& option);
        static LayoutRects buildLayoutRects(const QRect& rowRect);
        static void drawRowBackground(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rowRect);
        static void drawAvatar(QPainter* painter, const QByteArray& avatar, const QRect& avatarRect);
        static void drawMainText(QPainter* painter,
                                 const QString& name,
                                 const QString& summary,
                                 const QRect& nameRect,
                                 const QRect& summaryRect);
        static void drawSendTime(QPainter* painter, const QDateTime& sendTime, const QRect& timeRect);
        static void drawUnreadBadge(QPainter* painter, int unreadCount, const QRect& unreadBadgeRect);
    };


    class ChatSessionList : public QWidget
    {
        Q_OBJECT

    public:
        explicit ChatSessionList(QWidget* parent = nullptr);

    private:
        void initStyle();
        void initLayout();
        void setupConnections();

    private:
        QListView* listView = nullptr;
        ChatSessionListModel* listModel = nullptr;
        sys::relation::application::RelationApplicationService* relationApplicationService = QInjection::Inject;
    };
} // namespace ui::relation_widgets

