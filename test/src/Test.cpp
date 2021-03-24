#include <gtest/gtest.h>

#include "ExponentialsTestDouble.h"
#include "ExponentialsTestFloat.h"
#include "GeneralTestDouble.h"
#include "GeneralTestFloat.h"
#include "InterpolatorTest.h"
#include "TrigTestDouble.h"
#include "TrigTestFloat.h"
#include "Vec2Test.h"
#include "Vec3Test.h"

auto main(int argc, char** argv) noexcept -> int {
	testing::InitGoogleTest(&argc, argv);
	testing::FLAGS_gtest_death_test_style = "fast";
	return RUN_ALL_TESTS();
}
