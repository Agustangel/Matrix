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
#include "vector.hpp"

namespace linmath {

template <typename T>
class shell_matrix {
 private:
  std::size_t n_rows = 0;
  std::size_t n_cols = 0;

  containers::vector<T> m_buffer;

 public:
  shell_matrix(std::size_t rows, std::size_t cols, T val_ = T{})
      : n_rows{rows}, n_cols{cols}, m_buffer{rows * cols, val_} {}

  shell_matrix(std::size_t rows, std::size_t cols, T* frst, T* lst)
      : n_rows{rows}, n_cols{cols}, m_buffer{frst, lst} {}

  shell_matrix(std::size_t rows, std::size_t cols,
               std::initializer_list<T> list)
      : n_rows{rows}, n_cols{cols}, m_buffer{list.begin(), list.end()} {}

  static shell_matrix zero(std::size_t rows, std::size_t cols) {
    return shell_matrix{rows, cols};
  }

  static shell_matrix identity(std::size_t sz) {
    shell_matrix ret{sz, sz};
    // need iterator
    for (std::size_t i = 0; i != sz; ++i) {};
  }
};
}  // namespace linmath