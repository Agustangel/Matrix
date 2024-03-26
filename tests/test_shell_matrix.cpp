#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include "shell_matrix.hpp"

using sh_matrix = typename linmath::shell_matrix<float>;
using it = iterator::myIterator<float>;

TEST(test_shell_matrix, test_ctor_1) {
  std::size_t rows = 4;
  std::size_t cols = 4;
  std::initializer_list<float> list = {1.0, 2.0, 3.0, 4.0, 5.0,
                                       6.0, 7.0, 8.0, 9.0, 10.0};

  sh_matrix matr{rows, cols, list};
  ASSERT_EQ(matr.nrows(), rows);
  ASSERT_EQ(matr.ncols(), cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols && i * cols + j < 10; ++j)
      ASSERT_EQ(matr[i][j], *(list.begin() + i * cols + j));
  }
}

TEST(test_shell_matrix, test_zero) {
  sh_matrix matr = sh_matrix::zero(5, 4);
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 4; ++j)
      ASSERT_EQ(matr[i][j], 0.0);
}

TEST(test_shell_matrix, test_identity) {
  const sh_matrix matr = sh_matrix::identity(10);
  for (int i = 0; i < 10; i++)
    ASSERT_EQ(matr[i][i], 1.0);
}

TEST(test_shell_matrix, test_diag) {
  std::initializer_list<float> list = {1.0, 2.0, 3.0, 4.0, 5.0};
  const sh_matrix matr = sh_matrix::diag(3, list.begin(), list.end());
  for (int i = 0; i < 3; i++)
    ASSERT_EQ(matr[i][i], *(list.begin() + i));
}

TEST(test_shell_matrix, test_arithmetic) {
  const sh_matrix a = sh_matrix::identity(2);
  const sh_matrix b = {2, 2, {1, 2, 3, 4}};
  ASSERT_EQ(a + b, sh_matrix(2, 2, {2, 2, 3, 5}));
  ASSERT_EQ(b - a, sh_matrix(2, 2, {0, 2, 3, 3}));
  ASSERT_EQ(a / 2.0f, sh_matrix(2, 2, {0.5, 0, 0, 0.5}));
}

TEST(test_shell_matrix, test_mult_eq) {
  sh_matrix a = sh_matrix::identity(10);
  a *= 222;
  for (int i = 0; i < 10; i++)
    ASSERT_EQ(a[i][i], 222);
}

TEST(test_shell_matrix, test_multiplication) {
  sh_matrix a = sh_matrix::identity(10);
  sh_matrix b = a * 222.0f;
  for (int i = 0; i < 10; i++)
    ASSERT_EQ(b[i][i], 222);
}

TEST(test_shell_matrix, test_devision) {
  sh_matrix a = sh_matrix::identity(10);
  a *= 100;
  sh_matrix b = a / 5.0f;
  for (int i = 0; i < 10; ++i)
    ASSERT_EQ(b[i][i], 20);
}

TEST(test_shell_matrix, test_transponse_1) {
  sh_matrix a(4, 7, 0);
  a.transpose();
  ASSERT_EQ(a.ncols(), 4);
  ASSERT_EQ(a.nrows(), 7);
}

TEST(test_shell_matrix, test_transponse_2) {
  std::vector vals{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  sh_matrix a(4, 3, vals.begin(), vals.end());

  auto it = vals.begin();
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(a[i][j], *it);
      ++it;
    }
  sh_matrix b{3, 4, {1, 4, 7, 10, 2, 5, 8, 11, 3, 6, 9, 12}};
  auto c = a.transpose();
  EXPECT_TRUE(c == b);
}

TEST(test_shell_matrix, test_multiplication_1) {
  sh_matrix A(2, 2, {1, -2, 2, -4});
  sh_matrix B(2, 2, {2, 4, 1, 2});

  ASSERT_TRUE(A * B == sh_matrix::zero(2, 2));
}

TEST(test_shell_matrix, test_multiplication_2) {
  sh_matrix A{3, 3, {3, -1, 2, 4, 2, 1, -8, 3, 5}};
  sh_matrix B{3, 1, {5, -7, 2}};

  ASSERT_TRUE(A * B == sh_matrix(3, 1, {26, 8, -51}));
}

TEST(test_shell_matrix, test_multiplication_3) {
  sh_matrix A{3, 3, {5, 8, -4, 6, 9, -5, 4, 7, -3}};
  sh_matrix B{3, 1, {2, -3, 1}};

  auto C = A * B;
  ASSERT_TRUE(C == sh_matrix(3, 1, {-18, -20, -16}));
  ASSERT_TRUE(A != C);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}