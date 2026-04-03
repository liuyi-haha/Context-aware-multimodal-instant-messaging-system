#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QListView>
#include <QPixmap>
#include <QString>
#include <QStyledItemDelegate>
#include <QStyleOption>
#include <QWidget>

#include "dependencyinjector.h"
#include "sys/file-context/application/service/include/FileApplicationService.h"
#include "sys/relation-context/application/service/include/RelationApplicationService.h"


namespace ui::relation_widgets
{
    class FriendApplicationListModel : public QAbstractListModel
    {
    public:
        enum Roles
        {
            ApplicationIdRole = Qt::UserRole + 1,
            UserIdRole,
            AvatarRole,
            NameRole,
            VerificationMessageRole,
            StatusRole,
            ApplyTimeRole
        };

        explicit FriendApplicationListModel(QObject* parent = nullptr);
        int rowCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;

        // 业务方法:
        void insertViewByDateTimeDesc(const contract::relation::FriendApplicationView& friendApplication);
        // 由FriendApplicationList调用，因为FriendApplicationView是异步加载的，构造函数中暂不初始化
        void setViews(const QList<contract::relation::FriendApplicationView>& sortedFriendApplications);
        contract::relation::FriendApplicationView viewAt(int row) const;
        bool updateStatusByApplicationId(const QString& applicationId,
                                         contract::relation::FriendApplicationView::Status status);

    private:
        QList<contract::relation::FriendApplicationView> views;
        FileApplicationService* fileApplicationService = QInjection::Inject;

    private:
        void loadAvatarAsync(const QString& applicationId, const QString& avatarFileId);
        void updateAvatarByApplicationId(const QString& applicationId, const QByteArray& avatar);
    };


    class ApplicationModelDelegate : public QStyledItemDelegate
    {
    public:
        explicit ApplicationModelDelegate(QObject* parent = nullptr);
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;

    private:
        struct LayoutRects
        {
            QRect avatarRect;
            QRect nameRect;
            QRect messageRect;
            QRect statusRect;
        };

        static QColor rowBackground(const QStyleOptionViewItem& option);
        static LayoutRects buildLayoutRects(const QRect& rowRect);
        static void drawRowBackground(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rowRect);
        static void drawAvatar(QPainter* painter, const QByteArray& avatar, const QRect& avatarRect);
        static void drawMainText(QPainter* painter,
                                 const QString& name,
                                 const QString& message,
                                 const QRect& nameRect,
                                 const QRect& messageRect);
        static void drawStatusBadge(QPainter* painter, const QString& status, const QRect& statusRect);
    };


    class FriendApplicationList : public QWidget
    {
        Q_OBJECT

    public:
        explicit FriendApplicationList(QWidget* parent = nullptr);

    private:
        void initStyle();
        void initLayout();
        void setupConnections();

    private:
        QListView* listView;
        FriendApplicationListModel* listModel;
        sys::relation::application::RelationApplicationService* relationApplicationService = QInjection::Inject;
    };
} // namespace ui::relation_widgets
