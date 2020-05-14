#include "gtest/gtest.h"
#include <nlohmann/json.hpp>
#include "../include/foobar.hpp"

#include <vector>

using nlohmann::json;

class FooTests : public ::testing::Test {};

TEST_F(FooTests, Foo1) {
  ASSERT_EQ(foobar::add(1, 2), 3);

  // Everything is fine if you use a vector of doubles.
  json doubles_json = {1.1, 2.2, 3.3};
  std::vector<double> fetched_doubles = doubles_json.get<std::vector<double>>();

  // But you get the heap buffer overflow when using a vector of ints
  json ints_json = {1, 2, 3};

  // If you uncomment the line below then the buffer overflow also won't occur
  std::vector<int> fetched_ints = ints_json.get<std::vector<int>>();
}

TEST_F(FooTests, Foo2) {}
TEST_F(FooTests, Foo3) {}
TEST_F(FooTests, Foo4) {}

// If you uncomment the line below then the buffer overflow won't occur
TEST_F(FooTests, Foo5) {}
