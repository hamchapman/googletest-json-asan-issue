#include "gtest/gtest.h"
#include "../include/foobar.hpp"
#include "../include/json.hpp"

#include <vector>

using nlohmann::json;

class FooTests : public ::testing::Test {};

TEST_F(FooTests, Foo1) {
  ASSERT_EQ(foobar::add(1, 2), 3);

  // You get a heap buffer overflow when using a vector of ints
  json ints = {1, 2, 3};

  // If you uncomment the line below then the buffer overflow won't occur
  std::vector<int> ints_vec = ints.get<std::vector<int>>();
}

TEST_F(FooTests, Foo2) {}
TEST_F(FooTests, Foo3) {}
TEST_F(FooTests, Foo4) {}

// If you uncomment the line below then the buffer overflow won't occur
TEST_F(FooTests, Foo5) {}
