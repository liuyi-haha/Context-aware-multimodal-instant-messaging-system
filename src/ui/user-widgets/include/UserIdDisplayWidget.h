//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QWidget>

class QLabel;
class QPushButton;
class QString;

namespace ui::user_widgets
{
    class UserIdDisplayWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit UserIdDisplayWidget(const QString& userId, QWidget* parent = nullptr);

    private:
        void createSubWidgets();
        void initializeLayout();
        void initializeStyle();
        void setupConnections();
        void copyUserId();

    private:
        QString m_userId;
        QLabel* m_userIdLabel;
        QPushButton* m_copyButton;
        QPushButton* m_goToLoginButton;
    };
}
