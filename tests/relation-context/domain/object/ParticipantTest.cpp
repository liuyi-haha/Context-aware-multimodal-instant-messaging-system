//
// Created by 86150 on 2026/4/1.
//
#include <gtest/gtest.h>

#include "sys/common/exception/DomainException.h"
#include "sys/relation-context/domain/object/include/Participant.h"
#include "utils/ExceptionTestUtils.h"


class ParticipantTest : public testing::Test
{
};

TEST_F(ParticipantTest, 构造Participant时传入remark_调用hasValue应该返回true_调用remarkValue应该返回对应的备注值)
{
    const auto participant = sys::relation::domain::Participant::of(
        "P-100001", "CS-100001", "100000001", sys::relation::domain::ParticipantRole::PrivateMember, "my_remark");

    EXPECT_TRUE(participant->hasRemark());
    EXPECT_EQ(participant->remarkValue(), "my_remark");
}

TEST_F(ParticipantTest, 构造Participant时没有传入remark_调用hasValue应该返回false_调用remarkValue应该抛异常)
{
    auto participant = sys::relation::domain::Participant::of(
        "P-100001", "CS-100001", "100000001", sys::relation::domain::ParticipantRole::PrivateMember);

    EXPECT_FALSE(participant->hasRemark());
    tests::utils::expectThrowWithMessage<sys::core::DomainException>(
        [participant]
        {
            auto remark = participant->remarkValue();
        },
        "无权查看备注"
    );
}
