//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QWidget>

#include "dependencyinjector.h"
#include "sys/auth-context/application/service/include/AuthApplicationService.h"
#include "sys/notification-context/application/service/include/NotificationApplicationService.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QTimer;

namespace ui::auth_widgets
{
    class LoginWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit LoginWidget(
            QWidget* parent = nullptr,
            sys::auth::application::AuthApplicationService* authApplicationService = QInjection::Inject,
            sys::notification::application::NotificationApplicationService* notificationApplicationService = QInjection::Inject);

    private:
        void createSubWidgets();
        void initializeLayout();
        void initializeStyle();
        void setupConnections();
        void setSubmitting(bool submitting);
        void tickLoginLoading();
        void submitLogin();
        void goToRegister();

    private:
        QLabel* m_titleLabel;
        QLineEdit* m_phoneEdit;
        QLineEdit* m_passwordEdit;
        QPushButton* m_loginButton;
        QPushButton* m_goToRegisterLink;
        QTimer* m_loginLoadingTimer;
        bool m_isSubmitting;
        int m_loadingFrameIndex;
        sys::auth::application::AuthApplicationService* m_authApplicationService;
        sys::notification::application::NotificationApplicationService* m_notificationApplicationService;
    };
}