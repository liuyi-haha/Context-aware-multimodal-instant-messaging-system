//
// Created by 86150 on 2026/4/9.
//
#pragma once
#include "notifications.pb.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/common/component/Websocket.h"
#include "sys/message-context/application/include/MessageApplicationService.h"
#include "sys/starter/include/AppStarter.h"
#include "ui/auth-widgets/include/LoginWidget.h"
#include "ui/common-widgets/SoundPlayer.h"
#include "ui/main-widgets/include/MainWindow.h"
#include "ui/user-widgets/include/RegisterWidget.h"


// 职责：管理登录，注册，主窗口三个页面和后台
class Controller : public QObject
{
    Q_OBJECT

private:
    sys::relation::application::RelationApplicationService* relationApplicationService = QInjection::Inject;
    sys::notification::application::NotificationApplicationService* notificationApplicationService = QInjection::Inject;
    sys::message::application::MessageApplicationService* messageApplicationService = QInjection::Inject;

public:
    Controller()
    {
        // 订阅UIEventBus的信号
        auto* eventBus = ui::common::UIEventBus::instance();

        // 连接去登录信号
        connect(eventBus, &ui::common::UIEventBus::goToLoginRequested,
                this, [this]()
                {
                    handleGoToLogin();
                });

        // 连接去注册信号
        connect(eventBus, &ui::common::UIEventBus::goToRegisterRequested,
                this, [this]()
                {
                    handleGoToRegister();
                });

        // 连接登录成功信号
        connect(eventBus, &ui::common::UIEventBus::loginSuccess,
                this, [this]()
                {
                    ui::common_widgets::SoundPlayer::play();
                    showMainWindow();
                    tryToConnect();
                });

        // 连接Websocket的接收消息的信号
        connect(&Websocket::instance(), &Websocket::binaryMessageReceived,
                this, [this](const QByteArray& bytes)
                {
                    // 1,反序列化bytes，得到Notification对象
                    //FriendApplicationReceivedNotification notification; // 假设有FriendApplicationReceivedNotification类
                    // 2,根据Notification对象的类型，分发到不同的处理函数
                    proto::notification::WebSocketNotification notification;
                    assert(notification.ParseFromArray(bytes.data(), bytes.size()));

                    switch (notification.type())
                    {
                    case proto::notification::TYPE_FRIEND_APPLICATION_RECEIVED:
                        // 处理好友申请通知
                        relationApplicationService->handleFriendApplicationReceived(notification.payload().data());
                        break;
                    case proto::notification::TYPE_FRIEND_APPLICATION_HANDLED:
                        relationApplicationService->handleFriendApplicationHandled(notification.payload().data());
                        break;
                    case proto::notification::TYPE_MESSAGE_RECEIVED:
                        messageApplicationService->handleMessageReceived(notification.payload().data());
                        break;
                    default:
                        // 其他类型的通知，暂时不处理
                        qDebug() << "Controller say: 未知的通知类型:" << notification.type();
                        assert(false);
                        break;
                    }
                });
    }

    void start()
    {
        // 判断是否已登录
        auto& userCredentialManager = sys::common::component::UserCredentialManager::instance();
        if (userCredentialManager.getCurrentToken().isEmpty())
        {
            // 没有登录，显示登录/注册界面
            showLoginWidget();
        }
        else
        {
            // 已登录，显示主界面
            showMainWindow();
        }
    }

private:
    void handleGoToLogin()
    {
        // 切换到登录界面
        showLoginWidget();
    }

    void handleGoToRegister()
    {
        // 切换到注册界面
        showRegisterWidget();
    }

    void showLoginWidget()
    {
        // 关闭当前窗口（如果需要）
        // 显示登录界面
        auto* loginWidget = new ui::auth_widgets::LoginWidget; // 假设有LoginWidget类
        loginWidget->show();
        loginWidget->setAttribute(Qt::WA_DeleteOnClose);
    }

    void showRegisterWidget()
    {
        auto* registerWidget = new ui::user_widgets::RegisterWidget;
        registerWidget->show();
        registerWidget->setAttribute(Qt::WA_DeleteOnClose);
    }

    void showMainWindow()
    {
        auto* mainWindow = new ui::main_widgets::MainWindow;
        mainWindow->show();
        mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    }

    void tryToConnect()
    {
        // 同步调用通知上下文的尝试连接方法
        notificationApplicationService->tryToConnect();
    }
};
