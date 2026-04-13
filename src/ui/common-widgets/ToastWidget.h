//
// Created by 86150 on 2026/4/7.
//

#pragma once
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QPainter>
#include <QScreen>

namespace ui::common_widgets
{
    class ToastWidget : public QWidget
    {
        Q_OBJECT

    public:
        // 静态方法：显示一个简单的Toast提示（底部显示）
        static void showToast(QWidget* parent, const QString& text, int durationMs = 3000)
        {
            ToastWidget* toast = new ToastWidget(parent, text);
            toast->show();
            toast->startTimer(durationMs);
        }

    protected:
        void paintEvent(QPaintEvent* event) override
        {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);

            // 绘制半透明圆角背景
            painter.setBrush(QColor(40, 40, 40, 220));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(rect(), 8, 8);

            QWidget::paintEvent(event);
        }

    private slots:
        void onTimeout()
        {
            close();
            deleteLater();
        }

    private:
        ToastWidget(QWidget* parent, const QString& text)
            : QWidget(parent)
        {
            // 设置窗口属性：无边框、置顶、透明背景
            setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip | Qt::WindowStaysOnTopHint);
            setAttribute(Qt::WA_TranslucentBackground);
            setAttribute(Qt::WA_DeleteOnClose);

            // 创建布局和标签
            QVBoxLayout* layout = new QVBoxLayout(this);
            layout->setContentsMargins(20, 10, 20, 10);

            QLabel* label = new QLabel(text, this);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("QLabel { color: white; font-size: 14px; background: transparent; }");
            layout->addWidget(label);

            // 调整大小以适应内容
            adjustSize();

            // 计算底部居中位置
            if (parent)
            {
                // 获取父窗口的全局位置和大小
                QPoint parentGlobalPos = parent->mapToGlobal(QPoint(0, 0));
                int parentWidth = parent->width();
                int parentHeight = parent->height();

                // 计算X坐标：父窗口水平居中
                int x = parentGlobalPos.x() + (parentWidth - width()) / 2;
                // 计算Y坐标：距离父窗口底部40像素
                int y = parentGlobalPos.y() + parentHeight - height() - 40;

                move(x, y);
            }
            else
            {
                // 如果没有父窗口，在屏幕底部居中显示
                QRect screenGeometry = QApplication::primaryScreen()->geometry();
                int x = (screenGeometry.width() - width()) / 2;
                int y = screenGeometry.height() - height() - 50;
                move(x, y);
            }
        }

        void startTimer(int ms)
        {
            QTimer::singleShot(ms, this, &ToastWidget::onTimeout);
        }
    };
}
