//
// Created by 86150 on 2026/3/22.
//
#pragma once
#include <QDateTime>
#include <QSharedPointer>
#include <QString>

namespace sys::relation::domain
{
    class VerificationMessage
    {
    public:
        explicit VerificationMessage(const QString& message = QString());

        static bool checkVerificationMessage(const QString& value);
        const QString& value() const;

    private:
        QString message;
    };

    class RecipientRemark
    {
    public:
        explicit RecipientRemark(const QString& remark = QString());

        static bool checkRecipientRemark(const QString& value);
        const QString& value() const;

    private:
        QString remark;
    };

    class ApplyingInfo
    {
    public:
        ApplyingInfo(const QString& applicantUserId,
                     const QString& targetUserId,
                     const QDateTime& applyTime);

        const QString& applicantId() const;
        const QString& targetId() const;
        const QDateTime& appliedAt() const;

    private:
        QString applicantUserId;
        QString targetUserId;
        QDateTime applyTime;
    };

    enum class ApplicationStatus
    {
        Accepted,
        Rejected,
        Pending
    };

    class FriendApplication
    {
    public:
        static QSharedPointer<FriendApplication> of(const QString& applicantUserId,
                                                    const QString& friendApplicationId,
                                                    const QString& targetUserId,
                                                    const QString& verificationMessage,
                                                    const QString& recipientRemark,
                                                    const QDateTime& applyTime);
        static QList<QSharedPointer<FriendApplication>> composeAndSortByApplyTimeDesc(
            const QList<QSharedPointer<FriendApplication>>& friendApplications1,
            const QList<QSharedPointer<FriendApplication>>& friendApplications2);

        const QString& friendApplicationId() const;
        const QString& applicantUserId() const;
        const QString& targetUserId() const;
        const QString& verificationMessageValue() const;
        const QString& recipientRemarkValue() const;
        const QDateTime& applyTime() const;
        ApplicationStatus applicationStatus() const;
        void accept();
        void reject();

    private:
        FriendApplication(const QString& friendApplicationId, const QString& applicantUserId,
                          const QString& targetUserId,
                          const QString& verificationMessage, const QString& recipientRemark,
                          const QDateTime& applyTime);

    private:
        QString id;
        VerificationMessage verificationMessage;
        ApplyingInfo applyingInfo;
        RecipientRemark recipientRemark;
        ApplicationStatus status = ApplicationStatus::Pending;
    };
}
