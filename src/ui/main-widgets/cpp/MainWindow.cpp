#include "ui/main-widgets/include/MainWindow.h"
#include "ui/main-widgets/include/MiddleStack.h"
#include "ui/main-widgets/include/NavBar.h"
#include "ui/main-widgets/include/RightStack.h"
#include <QHBoxLayout>
#include <QSplitter>

namespace ui::main_widgets
{
    void MainWindow::initializeStyle()
    {
        setWindowTitle("即时通讯系统");
        resize(900, 700);
        setMinimumSize(800, 600);

        setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QSplitter::handle {
            background-color: #e5e5e5;
            width: 1px;
        }
    )");
    }

    void MainWindow::createSubWidgets()
    {
        m_navBar = new NavBar();
        m_middleStack = new MiddleStack();
        m_rightStack = new RightStack();
    }

    void MainWindow::initializeLayout()
    {
        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        QSplitter* mainSplitter = new QSplitter(Qt::Horizontal);
        mainSplitter->setHandleWidth(1); // 设置分割线宽度
        mainSplitter->setChildrenCollapsible(false); // 禁止子部件被完全折叠
        mainSplitter->setOpaqueResize(true); // 调整大小时立即重绘


        mainSplitter->addWidget(m_middleStack);
        mainSplitter->addWidget(m_rightStack);

        mainSplitter->setStretchFactor(0, 0);
        mainSplitter->setStretchFactor(1, 0);

        mainSplitter->setSizes({300, width() - 60 - 300});

        mainLayout->addWidget(m_navBar);
        mainLayout->addWidget(mainSplitter);
    }

    MainWindow::MainWindow(QWidget* parent)
        : QWidget(parent)
    {
        initializeStyle();
        createSubWidgets();
        initializeLayout();

        setupConnections();
    }

    void MainWindow::setupConnections()
    {
    }
} // namespace ui::main_widgets
