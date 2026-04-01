//
// Created by 86150 on 2026/3/22.
//

#include "sys/relation-context/domain/object/include/FriendApplication.h"

#include <algorithm>
#include <QRegularExpression>

#include "sys/relation-context/domain/exception/FriendApplicationAlreadyProcessedException.h"
#include "sys/relation-context/domain/exception/InvalidApplicantUserIdException.h"
#include "sys/relation-context/domain/exception/InvalidApplyTimeException.h"
#include "sys/relation-context/domain/exception/InvalidFriendApplicationIdException.h"
#include "sys/relation-context/domain/exception/InvalidRecipientRemarkException.h"
#include "sys/relation-context/domain/exception/InvalidTargetUserIdException.h"
#include "sys/relation-context/domain/exception/InvalidVerificationMessageException.h"

namespace sys::relation::domain
{
    VerificationMessage::VerificationMessage(const QString& message)
        : message(message)
    {
        if (!checkVerificationMessage(message))
        {
            throw InvalidVerificationMessageException();
        }
    }

    bool VerificationMessage::checkVerificationMessage(const QString& value)
    {
        // Verification message now accepts any character type, only enforce max length.
        return value.size() <= 20;
    }

    const QString& VerificationMessage::value() const
    {
        return message;
    }

    RecipientRemark::RecipientRemark(const QString& remark)
        : remark(remark)
    {
        if (!checkRecipientRemark(remark))
        {
            throw InvalidRecipientRemarkException();
        }
    }

    bool RecipientRemark::checkRecipientRemark(const QString& value)
    {
        static const QRegularExpression pattern("^[A-Za-z_\\x{4E00}-\\x{9FFF}]{0,20}$");
        return pattern.match(value).hasMatch();
    }

    const QString& RecipientRemark::value() const
    {
        return remark;
    }

    ApplyingInfo::ApplyingInfo(const QString& applicantUserId,
                               const QString& targetUserId,
                               const QDateTime& applyTime)
        : applicantUserId(applicantUserId),
          targetUserId(targetUserId),
          applyTime(applyTime)
    {
        if (applicantUserId.trimmed().isEmpty())
        {
            throw InvalidApplicantUserIdException();
        }
    }

    const QString& ApplyingInfo::applicantId() const
    {
        return applicantUserId;
    }

    const QString& ApplyingInfo::targetId() const
    {
        return targetUserId;
    }

    const QDateTime& ApplyingInfo::appliedAt() const
    {
        return applyTime;
    }

    FriendApplication::FriendApplication(const QString& friendApplicationId,
                                         const QString& applicantUserId,
                                         const QString& targetUserId,
                                         const QString& verificationMessage,
                                         const QString& recipientRemark,
                                         const QDateTime& applyTime)
        : id(friendApplicationId),
          verificationMessage(verificationMessage),
          applyingInfo(applicantUserId, targetUserId, applyTime),
          recipientRemark(recipientRemark),
          status(ApplicationStatus::Pending)
    {
        if (friendApplicationId.trimmed().isEmpty())
        {
            throw InvalidFriendApplicationIdException();
        }
    }

    QSharedPointer<FriendApplication> FriendApplication::of(const QString& applicantUserId,
                                                            const QString& friendApplicationId,
                                                            const QString& targetUserId,
                                                            const QString& verificationMessage,
                                                            const QString& recipientRemark,
                                                            const QDateTime& applyTime)
    {
        return QSharedPointer<FriendApplication>(new FriendApplication(
            friendApplicationId,
            applicantUserId,
            targetUserId,
            verificationMessage,
            recipientRemark,
            applyTime));
    }

    QList<QSharedPointer<FriendApplication>> FriendApplication::composeAndSortByApplyTimeDesc(
        const QList<QSharedPointer<FriendApplication>>& friendApplications1,
        const QList<QSharedPointer<FriendApplication>>& friendApplications2)
    {
        QList<QSharedPointer<FriendApplication>> merged;
        merged.reserve(friendApplications1.size() + friendApplications2.size());

        for (const auto& application : friendApplications1)
        {
            if (application != nullptr)
            {
                merged.push_back(application);
            }
        }

        for (const auto& application : friendApplications2)
        {
            if (application != nullptr)
            {
                merged.push_back(application);
            }
        }

        std::sort(merged.begin(), merged.end(), [](const auto& left, const auto& right)
        {
            if (left == nullptr)
            {
                return false;
            }
            if (right == nullptr)
            {
                return true;
            }
            return left->applyTime() > right->applyTime();
        });

        return merged;
    }

    const QString& FriendApplication::friendApplicationId() const
    {
        return id;
    }

    const QString& FriendApplication::applicantUserId() const
    {
        return applyingInfo.applicantId();
    }

    const QString& FriendApplication::targetUserId() const
    {
        return applyingInfo.targetId();
    }

    const QString& FriendApplication::verificationMessageValue() const
    {
        return verificationMessage.value();
    }

    const QString& FriendApplication::recipientRemarkValue() const
    {
        return recipientRemark.value();
    }

    const QDateTime& FriendApplication::applyTime() const
    {
        return applyingInfo.appliedAt();
    }

    ApplicationStatus FriendApplication::applicationStatus() const
    {
        return status;
    }

    void FriendApplication::accept()
    {
        if (status != ApplicationStatus::Pending)
        {
            throw FriendApplicationAlreadyProcessedException();
        }
        status = ApplicationStatus::Accepted;
    }

    void FriendApplication::reject()
    {
        if (status != ApplicationStatus::Pending)
        {
            throw FriendApplicationAlreadyProcessedException();
        }
        status = ApplicationStatus::Rejected;
    }
}


