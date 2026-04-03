#include "ui/relation-widgets/include/FriendApplicationList.h"

#include <QCoreApplication>
#include <QFutureWatcher>
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include <QVBoxLayout>
#include <algorithm>

#include <QtConcurrent/QtConcurrent>
#include "contract/system-provider/relation-context-provider/FriendApplicationView.hpp"
#include "dependencyinjector.h"
#include "sys/file-context/application/service/include/FileApplicationService.h"
#include "ui/common/UIEVentBus.h"

namespace ui::relation_widgets
{
    FriendApplicationListModel::FriendApplicationListModel(QObject* parent)
        : QAbstractListModel(parent)
    {
    }

    int FriendApplicationListModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
        {
            return 0;
        }

        return views.size();
    }

    QVariant FriendApplicationListModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= views.size())
        {
            return {};
        }

        const auto& model = views.at(index.row());
        switch (role)
        {
        case Qt::DisplayRole:
        case NameRole:
            return model.name;
        case ApplicationIdRole:
            return model.applicationId;
        case UserIdRole:
            return model.peerUserId;
        case AvatarRole:
            return model.peerUserAvatar.value_or(QByteArray());
        case VerificationMessageRole:
            return model.verificationMessage;
        case StatusRole:
            return contract::relation::FriendApplicationView::statusToText(model.status);
        case ApplyTimeRole:
            return model.applyTime;
        default:
            return {};
        }
    }

    // 别忘了异步加载头像
    void FriendApplicationListModel::insertViewByDateTimeDesc(
        const contract::relation::FriendApplicationView& friendApplication)
    {
        int insertRow = 0;
        while (insertRow < views.size() && views.at(insertRow).applyTime >= friendApplication.applyTime)
        {
            ++insertRow;
        }

        beginInsertRows(QModelIndex(), insertRow, insertRow);
        views.insert(insertRow, friendApplication);
        endInsertRows();

        loadAvatarAsync(friendApplication.applicationId, friendApplication.peerUserAvatarFileId);
    }

    // 对于每个view，都要异步加载头像
    //
    void FriendApplicationListModel::setViews(
        const QList<contract::relation::FriendApplicationView>& sortedFriendApplications)
    {
        beginResetModel();
        views = sortedFriendApplications;
        endResetModel();

        for (const auto& view : sortedFriendApplications)
        {
            loadAvatarAsync(view.applicationId, view.peerUserAvatarFileId);
        }
    }

    contract::relation::FriendApplicationView FriendApplicationListModel::viewAt(const int row) const
    {
        if (row < 0 || row >= views.size())
        {
            return {};
        }

        return views.at(row);
    }


    void FriendApplicationListModel::updateAvatarByApplicationId(const QString& applicationId, const QByteArray& avatar)
    {
        for (int row = 0; row < views.size(); ++row)
        {
            if (views[row].applicationId != applicationId)
            {
                continue;
            }

            views[row].peerUserAvatar = avatar;
            const auto index = this->index(row, 0);
            emit dataChanged(index, index);
            return;
        }
    }

    bool FriendApplicationListModel::updateStatusByApplicationId(
        const QString& applicationId,
        const contract::relation::FriendApplicationView::Status status)
    {
        for (int row = 0; row < views.size(); ++row)
        {
            if (views[row].applicationId != applicationId)
            {
                continue;
            }

            views[row].status = status;
            const auto index = this->index(row, 0);
            emit dataChanged(index, index, {StatusRole});
            return true;
        }

        return false;
    }

    void FriendApplicationListModel::loadAvatarAsync(const QString& applicationId, const QString& avatarFileId)
    {
        qDebug() << "Start loading avatar for applicationId:" << applicationId << "avatarFileId:" << avatarFileId;;
        QtConcurrent::run([avatarFileId, service = fileApplicationService]()-> QByteArray
        {
            if (service == nullptr)
            {
                return {};
            }
            return service->getFile(avatarFileId);
        }).then(this, [this, applicationId](const QByteArray& data)
        {
            qDebug() << "Avatar loaded for applicationId:" << applicationId << "data size:" << data.size();
            if (!data.isNull())
            {
                updateAvatarByApplicationId(applicationId, data);
            }
        });
    }

    ApplicationModelDelegate::ApplicationModelDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    void ApplicationModelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        const QRect rowRect = option.rect.adjusted(2, 2, -2, -2);
        const auto rects = buildLayoutRects(rowRect);

        drawRowBackground(painter, option, rowRect);
        drawAvatar(painter, index.data(FriendApplicationListModel::AvatarRole).toByteArray(),
                   rects.avatarRect);
        drawMainText(painter,
                     index.data(FriendApplicationListModel::NameRole).toString(),
                     index.data(FriendApplicationListModel::VerificationMessageRole).toString(),
                     rects.nameRect,
                     rects.messageRect);
        drawStatusBadge(painter, index.data(FriendApplicationListModel::StatusRole).toString(), rects.statusRect);

        painter->restore();
    }

    QSize ApplicationModelDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
    {
        return {0, 78};
    }

    QColor ApplicationModelDelegate::rowBackground(const QStyleOptionViewItem& option)
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

    ApplicationModelDelegate::LayoutRects ApplicationModelDelegate::buildLayoutRects(const QRect& rowRect)
    {
        constexpr int leftPadding = 8;
        constexpr int avatarSize = 42;
        constexpr int gap = 8;
        constexpr int rightPadding = 8;
        constexpr int statusWidth = 84;

        LayoutRects rects;
        rects.avatarRect = QRect(rowRect.left() + leftPadding,
                                 rowRect.top() + (rowRect.height() - avatarSize) / 2,
                                 avatarSize,
                                 avatarSize);

        const int textLeft = rects.avatarRect.right() + gap;
        const int textRight = rowRect.right() - rightPadding - statusWidth - gap;

        rects.nameRect = QRect(textLeft, rowRect.top() + 14, textRight - textLeft, 22);
        rects.messageRect = QRect(textLeft, rowRect.top() + 38, textRight - textLeft, 20);
        rects.statusRect = QRect(rowRect.right() - rightPadding - statusWidth,
                                 rowRect.top() + (rowRect.height() - 28) / 2,
                                 statusWidth,
                                 28);

        return rects;
    }

    void ApplicationModelDelegate::drawRowBackground(QPainter* painter, const QStyleOptionViewItem& option,
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

    void ApplicationModelDelegate::drawAvatar(QPainter* painter, const QByteArray& avatar, const QRect& avatarRect)
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

    void ApplicationModelDelegate::drawMainText(QPainter* painter, const QString& name, const QString& message,
                                                const QRect& nameRect, const QRect& messageRect)
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
        painter->drawText(messageRect, Qt::AlignLeft | Qt::AlignVCenter, message);
    }

    void ApplicationModelDelegate::drawStatusBadge(QPainter* painter, const QString& status, const QRect& statusRect)
    {
        QColor textColor(132, 132, 132);
        QColor bgColor(245, 245, 245);

        if (status == QStringLiteral("待处理"))
        {
            textColor = QColor(5, 171, 75);
            bgColor = QColor(232, 248, 236);
        }
        else if (status == QStringLiteral("已同意"))
        {
            textColor = QColor(38, 125, 255);
            bgColor = QColor(232, 241, 255);
        }
        else if (status == QStringLiteral("已拒绝"))
        {
            textColor = QColor(178, 95, 95);
            bgColor = QColor(251, 239, 239);
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(bgColor);
        painter->drawRoundedRect(statusRect, 14, 14);

        QFont statusFont = painter->font();
        statusFont.setFamily(QStringLiteral("Microsoft YaHei"));
        statusFont.setPixelSize(12);
        statusFont.setWeight(QFont::Medium);
        painter->setFont(statusFont);
        painter->setPen(textColor);
        painter->drawText(statusRect, Qt::AlignCenter, status);
    }


    FriendApplicationList::FriendApplicationList(QWidget* parent)
        : QWidget(parent),
          listView(new QListView(this)),
          listModel(new FriendApplicationListModel(this))
    {
        initLayout();
        initStyle();

        listView->setModel(listModel);
        listView->setItemDelegate(new ApplicationModelDelegate(listView));


        setupConnections();

        // 异步加载好友申请列表
        QtConcurrent::run([this]()-> QList<contract::relation::FriendApplicationView>
        {
            return relationApplicationService->getFriendApplications();
        }).then(this, [this](const QList<contract::relation::FriendApplicationView>& views)
        {
            listModel->setViews(views);
        });
    }


    void FriendApplicationList::initStyle()
    {
        setObjectName(QStringLiteral("friendApplicationList"));
        setStyleSheet(
            "#friendApplicationList {"
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

    void FriendApplicationList::initLayout()
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

    void FriendApplicationList::setupConnections()
    {
        connect(listView, &QListView::clicked, this, [this](const QModelIndex& index)
        {
            emit ui::common::UIEventBus::instance()->friendApplicationSelected(listModel->viewAt(index.row()));
        });

        connect(ui::common::UIEventBus::instance(), &ui::common::UIEventBus::friendApplicationStatusChanged,
                this,
                [this](const QString& applicationId, const contract::relation::FriendApplicationView::Status status)
                {
                    listModel->updateStatusByApplicationId(applicationId, status);
                });
    }
}
