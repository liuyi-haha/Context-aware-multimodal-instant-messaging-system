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
#include "sys/common/component/UserCredentialManager.h"
#include "sys/relation-context/domain/object/include/FriendApplication.h"
#include "sys/relation-context/port/repository/include/FriendApplicationRepository.h"
#include "sys/starter/include/AppStarter.h"
#include "ui/user-widgets/include/RegisterWidget.h"

void seedApplication(const QString& friendApplicationId,
                     const QString& applicantUserId,
                     const QString& targetUserId,
                     const QString& verificationMessage,
                     const QString& recipientRemark,
                     const QDateTime& applyTime,
                     const sys::relation::domain::ApplicationStatus status)
{
    const auto application = sys::relation::domain::FriendApplication::of(
        applicantUserId,
        friendApplicationId,
        targetUserId,
        verificationMessage,
        recipientRemark,
        applyTime);

    QInjection::Pointer<sys::relation::port::FriendApplicationRepository> friendApplicationRepository;
    if (status == sys::relation::domain::ApplicationStatus::Accepted)
    {
        application->accept();
    }
    else if (status == sys::relation::domain::ApplicationStatus::Rejected)
    {
        application->reject();
    }

    friendApplicationRepository->save(application);
}

void setDatas()
{
    seedApplication("FA-MIX-5", "100000001", "100000002", "v5", "self_pending",
                    QDateTime(QDate(2026, 3, 30), QTime(15, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Pending);
    seedApplication("FA-MIX-4", "100000003", "100000001", "vfour", "other_pending",
                    QDateTime(QDate(2026, 3, 30), QTime(14, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Pending);
    seedApplication("FA-MIX-3", "100000001", "100000004", "v3", "self_accepted",
                    QDateTime(QDate(2026, 3, 30), QTime(13, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Accepted);
    seedApplication("FA-MIX-2", "100000002", "100000001", "v2", "other_rejected",
                    QDateTime(QDate(2026, 3, 30), QTime(12, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Rejected);
    seedApplication("FA-MIX-1", "100000004", "100000001", "v1", "other_accepted",
                    QDateTime(QDate(2026, 3, 30), QTime(11, 0, 0), Qt::UTC),
                    sys::relation::domain::ApplicationStatus::Accepted);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    // 设置userId和token，模拟用户已登录
    sys::common::component::UserCredentialManager::instance().update("100000001", "mock_token");
    sys::starter::AppStarter::bootstrap();
    //setDatas();
    //auto w = new ui::main_widgets::MainWindow();
    //auto w = new ui::main_widgets::MainWindow();
    // auto w = new ui::user_widgets::UserIdDisplayWidget("dfaffadfafe");
    //w->show();
    return a.exec();
}

// int a = 10;
//
// class Abstract : public QObject
// {
//     Q_OBJECT
//
// public:
//     virtual void doSomething() = 0;
//     virtual ~Abstract() = default;
// };
//
// class Myclasss : public Abstract
// {
//     Q_OBJECT
//
// public:
//     Myclasss()
//     {
//         qDebug() << "Myclasss" << a++;
//     }
//
//     void doSomething() override
//     {
//         qDebug() << "MyClass is doing something!";
//     }
//
//     ~Myclasss() override
//     {
//         qDebug() << "Myclasss析构了";
//     }
// };
//
// void foo(Abstract* object = QInjection::Inject)
// {
//     // do something with object
// }
//
// int main(int argc, char* argv[])
// {
//     QCoreApplication a(argc, argv);
//
//     QInjection::addSingleton(new Myclasss());
//
//     QInjection::Pointer<Myclasss> ptr;
//     ptr->doSomething();
//     Myclasss* p = ptr.data();
//     p = new Myclasss();
//     return a.exec();
// }
//
// #include "Main.moc"
