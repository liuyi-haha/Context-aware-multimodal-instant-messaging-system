#include "ui/main-widgets/include/MainWindow.h"
#include "ui/main-widgets/include/MiddleStack.h"
#include "ui/main-widgets/include/NavBar.h"
#include "ui/main-widgets/include/RightStack.h"
#include <QHBoxLayout>
#include <QSplitter>

#include "ui/main-widgets/include/MiddlePanel.h"

namespace ui::main_widgets
{
    // MainWindow 实现
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
        m_navBar = new NavBar(this);
        m_middlePanel = new MiddlePanel(this);
        m_rightStack = new RightStack(this);
    }

    void MainWindow::initializeLayout()
    {
        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        QSplitter* mainSplitter = new QSplitter(Qt::Horizontal);
        mainSplitter->setHandleWidth(1);
        mainSplitter->setChildrenCollapsible(false);
        mainSplitter->setOpaqueResize(true);

        mainSplitter->addWidget(m_middlePanel);
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
        // TODO: 连接搜索框的 textChanged 信号到搜索处理函数
        // TODO: 连接创建群聊按钮的 clicked 信号到创建群聊处理函数
    }
}
