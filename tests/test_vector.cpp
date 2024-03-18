#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include "vector.hpp"

static constexpr std::size_t default_capacity = 8;
using vector = typename containers::vector<int>;

class VectorFixture : public ::testing::Test {
 public:
  vector* my_vec;
  const std::size_t cap = 10;

 protected:
  void SetUp() override { my_vec = new containers::vector<int>{cap}; }
  void TearDown() override { delete my_vec; }
};

TEST(test_vector, test_ctor_1) {
  vector my_vec;
  ASSERT_EQ(my_vec.size(), 0);
  ASSERT_TRUE(my_vec.empty());
  ASSERT_EQ(my_vec.capacity(), default_capacity);
}

TEST(test_vector, test_ctor_2) {
  vector my_vec{10};
  ASSERT_EQ(my_vec.size(), 0);
  ASSERT_TRUE(my_vec.empty());
  ASSERT_EQ(my_vec.capacity(), 10);
}

TEST(test_vector, test_ctor_3) {
  std::size_t sz = 10;
  vector my_vec{sz, 1};
  ASSERT_EQ(my_vec.size(), sz);
  ASSERT_EQ(my_vec.capacity(), sz);

  for (std::size_t i = 0; i < sz; ++i)
    ASSERT_EQ(my_vec[i], 1);
}

TEST(test_vector, test_uptr) {
  containers::vector<std::unique_ptr<int>> vec;
  for (std::size_t i = 0; i < 100000; ++i)
    vec.push_back(std::make_unique<int>(i));
  for (std::size_t i = 0; i < 100000; ++i)
    EXPECT_EQ(*vec[i], i);
}

TEST_F(VectorFixture, push_pop_back) {
  for (std::size_t i = 0; i < 5; ++i) {
    my_vec->push_back(i);
    EXPECT_EQ(my_vec->size(), i + 1);
  }
  for (std::size_t j = 4; j > 0; --j) {
    EXPECT_EQ(my_vec->top(), j);
    my_vec->pop();
    EXPECT_EQ(my_vec->size(), j);
  }
}

TEST_F(VectorFixture, test_reserve) {
  std::size_t new_cap = cap * 2;
  my_vec->reserve(new_cap);
  EXPECT_EQ(my_vec->capacity(), new_cap);
}

TEST_F(VectorFixture, test_clear) {
  my_vec->clear();
  EXPECT_TRUE(my_vec->empty());
}

TEST_F(VectorFixture, test_iterator) {
  for (auto i = 0; i < 10; i++)
    my_vec->push_back(i);
  int i = 0;
  for (auto it = my_vec->begin(); it != my_vec->end(); ++it) {
    EXPECT_EQ(*it, i);
    ++i;
  }
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}