#include "gtest/gtest.h"
#include "../include/foobar.hpp"
#include "../include/json.hpp"

#include <vector>

using nlohmann::json;

class FooTests : public ::testing::Test {};

TEST_F(FooTests, Foo1) {}
TEST_F(FooTests, Foo2) {}
TEST_F(FooTests, Foo3) {}

// If you uncomment the line below then the buffer overflow won't occur
TEST_F(FooTests, Foo4) {}

TEST_F(FooTests, Foo) {
  ASSERT_EQ(foobar::add(1, 2), 3);

  json ints = json{1, 2, 3};

  // Or, if you uncomment the line below then the buffer overflow also won't
  // occur
  std::vector<int> ints_vec = ints.get<std::vector<int>>();
}
