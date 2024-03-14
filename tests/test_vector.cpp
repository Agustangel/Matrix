#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include "vector.hpp"

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}