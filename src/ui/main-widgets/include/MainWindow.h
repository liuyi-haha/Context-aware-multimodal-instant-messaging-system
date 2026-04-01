#pragma once
#include <QWidget>

namespace ui::main_widgets {

    class NavBar;
    class MiddleStack;
    class RightStack;

    class MainWindow : public QWidget
    {
        Q_OBJECT

    public:
        void initializeStyle();
        void createSubWidgets();
        void initializeLayout();
        explicit MainWindow(QWidget *parent = nullptr);

    private:
        void setupConnections();

    private:
        NavBar* m_navBar;
        MiddleStack* m_middleStack;
        RightStack* m_rightStack;
    };

}