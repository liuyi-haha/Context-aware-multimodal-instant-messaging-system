// #include <QApplication>
//
// #include "sys/starter/include/AppStarter.h"
// #include "ui/auth-widgets/include/LoginWidget.h"
//
// int main(int argc, char* argv[])
// {
//     QApplication app(argc, argv);
//
//     sys::starter::AppStarter::bootstrap();
//
//     auto* window = new ui::auth_widgets::LoginWidget(nullptr);
//     window->show();
//
//     return app.exec();
// }

#include "dependencypool.h"
#include "dependencyinjector.h"


#include <QApplication>

#include "dependencypointer.h"
#include "common/EventBus.h"
#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/object/include/FriendApplication.h"
#include "sys/relation-context/port/repository/include/FriendApplicationRepository.h"
#include "sys/starter/include/AppStarter.h"
#include "ui/auth-widgets/include/LoginWidget.h"
#include "ui/common/UIEVentBus.h"
#include "ui/main-widgets/include/MainWindow.h"
#include "ui/relation-widgets/include/FriendApplicationList.h"
#include "ui/main-widgets/include/MiddleStack.h"
#include "ui/user-widgets/include/RegisterWidget.h"
#include <QObject>

#include "Controller.h"
#include "ui/relation-widgets/include/FriendAddingWidget.h"


int main(int argc, char* argv[])
{
    // 设置currentUserId，模拟用户登录状态
    // sys::common::component::UserCredentialManager::instance().update("100000001", "mock_token", "刘奕",
    //                                                                  "刘奕的头像", "description",
    //                                                                  "1234567890"
    // );
    QApplication a(argc, argv);

    sys::starter::AppStarter::bootstrap();
    // sys::common::component::UserCredentialManager::instance().update("506647664",
    //                                                                  "6afb09fc39aeb7894d8702fa6cb1290f74b1b737a83445f717f5e75cdce2da98",
    //                                                                  "发送者",
    //                                                                  "2QsKiugULKa", "",
    //                                                                  "15091772572"
    // );
    Controller controller;
    controller.start();
    ui::common_widgets::SoundPlayer::play();

    // contract::user::UserView userView;
    // userView.userId = "100000001";
    // userView.nickname = "刘奕";
    // userView.avatarFileId = "毛毛的头像";
    // auto widget = new ui::user_widgets::FriendAddingWidget(userView);
    // widget->show();
    return a.exec();
}


// class Base : public QObject
// {
//     Q_OBJECT
//
// public:
//     virtual void func() = 0;
// };
//
// class Derived : public Base
// {
//     void func() override
//     {
//         qDebug() << "Derived func";
//     }
//
// public:
//     void say()
//     {
//         qDebug() << "hello";
//     }
// };
//
// void test()
// {
//     QInjection::addSingleton(new Derived);
//     Base* basePtr = QInjection::Inject;
//     //QInjection::Pointer<Base> basePtr;
//
//     static_cast<Derived*>(basePtr)->say();
// }
