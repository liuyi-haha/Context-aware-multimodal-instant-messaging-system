#pragma once
#include <QWidget>

#include "MiddlePanel.h"

namespace ui::main_widgets
{
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
        explicit MainWindow(QWidget* parent = nullptr);

    private:
        void setupConnections();

    private:
        NavBar* m_navBar;
        RightStack* m_rightStack;
        MiddlePanel* m_middlePanel;
    };
}
