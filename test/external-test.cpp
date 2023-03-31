#include <cstdio>
#include <gtest/gtest.h>

extern "C" {
#include "stdio.h"
}

TEST(CSAPPTEST, INVOCATIONTEST) {
  EXPECT_EQ(2, 2);
}
