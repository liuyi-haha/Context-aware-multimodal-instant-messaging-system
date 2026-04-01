//
// Created by 86150 on 2026/3/23.
//
#pragma once
#include <QWidget>

#include "dependencyinjector.h"
#include "sys/user-context/application/service/include/UserApplicationService.h"

class QLabel;
class QLineEdit;
class QObject;
class QEvent;
class QPushButton;
class QString;
class QTimer;

namespace ui::user_widgets
{
    class RegisterWidget: public QWidget
    {
        Q_OBJECT

    public:
        explicit RegisterWidget(
            QWidget* parent = nullptr,
            sys::user::application::UserApplicationService* userApplicationService = QInjection::Inject);

    private:
        bool eventFilter(QObject* watched, QEvent* event) override;
        void createSubWidgets();
        void initializeLayout();
        void initializeStyle();
        void setupConnections();
        void updateValidationState();
        void setSubmitting(bool submitting);
        void tickRegisterLoading();

        static bool isNicknameValid(const QString& nickname);
        static bool isPhoneValid(const QString& phone);
        static bool isPasswordValid(const QString& password);

        void chooseAvatar();
        void submitRegister();
        void showUserIdDisplay(const QString& userId);
        static QString generateMockUserId();
        void goToLogin();

    private:
        QLabel* m_avatarPreview;
        QLineEdit* m_phoneEdit;
        QLineEdit* m_passwordEdit;
        QLineEdit* m_nicknameEdit;
        QLabel* m_nicknameHintLabel;
        QLabel* m_phoneHintLabel;
        QLabel* m_passwordHintLabel;
        QPushButton* m_registerButton;
        QPushButton* m_goToLoginLink;
        QString m_avatarFilePath;
        QTimer* m_registerLoadingTimer;
        bool m_isSubmitting;
        int m_loadingFrameIndex;
        sys::user::application::UserApplicationService* m_userApplicationService;
    };
}