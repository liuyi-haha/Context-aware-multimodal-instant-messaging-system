#pragma once
#include <QString>
#include <QWidget>

class QPushButton;

namespace ui::main_widgets
{
    class NavBar : public QWidget
    {
        Q_OBJECT

    public:
        void initStyle();
        void createSubWidgets();
        void initLayout();
        explicit NavBar(QWidget* parent = nullptr);

    private:
        QPushButton* createNavButton(const QString& iconPath, const QString& tooltip, int index);
        void setActiveButton(QPushButton* activeButton);

    private:
        QPushButton* m_btnRobot;
        QPushButton* m_btnMessage;
        QPushButton* m_btnFriendApplication;
        QPushButton* m_btnFriend;
        QPushButton* m_btnGroup;
    };
} // namespace ui::main_widgets
