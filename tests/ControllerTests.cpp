
#include <gtest/gtest.h>

#include "ControllerSetup.hpp"

// Verify heating below the target temperature
TEST(ManagedControllerTest, HeatsBelowTarget)
{
    auto host = createHost();
    auto controller = host.createController(50.0);

    EXPECT_DOUBLE_EQ(
        controller->calculatePower(30.0),
        100.0
    );
}

// Verify no heating above the target temperature
TEST(ManagedControllerTest, StopsAboveTarget)
{
    auto host = createHost();
    auto controller = host.createController(50.0);

    EXPECT_DOUBLE_EQ(
        controller->calculatePower(60.0),
        0.0
    );
}

// Verify independent managed controller instances
TEST(ManagedControllerTest, IndependentInstances)
{
    auto host = createHost();

    auto first = host.createController(50.0);
    auto second = host.createController(80.0);

    EXPECT_DOUBLE_EQ(first->calculatePower(60.0), 0.0);
    EXPECT_DOUBLE_EQ(second->calculatePower(60.0), 100.0);
}
