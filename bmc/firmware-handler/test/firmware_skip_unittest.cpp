#include "skip_action.hpp"
#include "status.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ipmi_flash
{
namespace
{

TEST(SkipActionTest, ValidateTriggerReturnsTrue)
{
    SkipAction skip;
    EXPECT_TRUE(skip.trigger());
    size_t i = 0;
    skip.setCallback([&](TriggerableActionInterface&) { i++; });
    EXPECT_TRUE(skip.trigger());
    EXPECT_EQ(1, i);
    EXPECT_TRUE(skip.trigger());
    EXPECT_EQ(2, i);
}

TEST(SkipActionTest, ValidateStatusAlwaysSuccess)
{
    SkipAction skip;
    EXPECT_EQ(ActionStatus::success, skip.status());
    EXPECT_TRUE(skip.trigger());
    EXPECT_EQ(ActionStatus::success, skip.status());
}

TEST(SkipActionTest, AbortHasNoImpactOnStatus)
{
    SkipAction skip;
    EXPECT_EQ(ActionStatus::success, skip.status());
    skip.abort();
    EXPECT_EQ(ActionStatus::success, skip.status());
}

} // namespace
} // namespace ipmi_flash
