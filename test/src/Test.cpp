#include "test/ExponentialsTestFloat.h"
#include "test/ExponentialsTestDouble.h"
#include "test/GeneralTestFloat.h"
#include "test/GeneralTestDouble.h"
#include "test/TrigTestFloat.h"
#include "test/TrigTestDouble.h"
#include "test/InterpolatorTest.h"
#include "Vec2Test.h"
#include "Vec3Test.h"
#include <gtest/gtest.h>

auto main(int argc, char** argv) noexcept -> int {
	testing::InitGoogleTest(&argc, argv);
	testing::FLAGS_gtest_death_test_style = "fast";
	return RUN_ALL_TESTS();
}

