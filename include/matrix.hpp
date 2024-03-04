#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include "iterator.hpp"
#include "shell_matrix.hpp"
#include "vector.hpp"

namespace linmath {
template <typename T>
class matrix {
 private:
  shell_matrix<T> m_shell_matrix;
  containers::vector<T> rows_vec;

 public:
  using it = iterator::myIterator<T>;

  matrix(std::size_t rows, std::size_t cols, T val = T{})
      : m_shell_matrix{rows, cols, val} {
    reserve_rows_vec();
  }

  matrix(std::size_t rows, std::size_t cols, T* frst, T* lst)
      : m_shell_matrix{rows, cols, frst, lst} {
    reserve_rows_vec();
  }

  matrix(std::size_t rows, std::size_t cols, std::initializer_list<T> list)
      : m_shell_matrix{rows, cols, list} {
    reserve_rows_vec();
  }

  matrix(shell_matrix<T>&& rhs) : m_shell_matrix(std::move(rhs)) {
    reserve_rows_vec();
  }

  static matrix zero(std::size_t rows, std::size_t cols) {
    return matrix<T>{rows, cols};
  }

  static matrix identity(std::size_t sz) {
    return matrix{std::move(shell_matrix<T>::identity(sz))};
  }

  static matrix diag(std::size_t sz, it start, it finish) {
    return matrix {
      std::move(shell_matrix<T>::diag(sz, start, finish));
    }
  }

 private:
  void reserve_rows_vec() { rows_vec.expand(nrows()); }

 public:
  std::size_t nrows() const { return shell_matrix.rows(); }
  std::size_t ncols() const { return shell_matrix.cols(); }

  bool square() const { return nrows() == ncols(); }

  it begin() const { return shell_matrix.begin(); }
  it end() const { return shell_matrix.end(); }
};
}  // namespace linmath