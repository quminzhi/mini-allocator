#include <gtest/gtest.h>

extern "C" {
#include <stdlib.h>
}

class MMTest: public ::testing::Test {
protected:

  void SetUp() override {
  }

  void TearDown() override {}
};

TEST_F(MMTest, TestWordReference) {
  // do your test
}
