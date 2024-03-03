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
#include "vector.hpp"

namespace linmath {

template <typename T>
class shell_matrix {
 private:
  std::size_t n_rows = 0;
  std::size_t n_cols = 0;

  containers::vector<T> m_buffer;

 public:
  using it = iterator::myIterator<T>;

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
    it leap = ret.begin();
    for (std::size_t i = 0; i != sz; ++i, leap += sz + 1) {
      *leap = 1;
    };
    return ret;
  }

 private:
  class proxy_row {
   private:
    T *row_ptr, row_end_ptr;

   public:
    proxy_row() = default;
    proxy_row(const T* begin_ptr, std::size_t cols)
        : row_ptr{begin_ptr}, row_end_ptr{row_ptr + cols} {}

    T& operator[](std::size_t idx) { return row_ptr[idx]; }
    const T& operator[](std::size_t idx) const { return row_ptr[idx]; }

    it begin() const { return it{row_ptr}; }
    it end() const { return it{row_end_ptr}; }

    std::size_t size() const { return row_ptr - row_end_ptr; }
  };

 public:
  proxy_row& operator[](unsigned idx) {
    return proxy_row{&m_buffer[idx * n_cols], n_cols};
  }
  const proxy_row& operator[](unsigned idx) const {
    return proxy_row{&m_buffer[idx * n_cols], n_cols};
  }

  T trace() const {
    if (n_rows != n_cols)
      throw std::runtime_error("Cannot get trace of non-square matrix");

    T trace{};
    it leap = begin();
    for (std::size_t idx = 0; idx != n_cols; ++idx, leap += n_cols + 1) {
      trc += *leap;
    }
    return trace;
  }

  shell_matrix& transpose() & {
    static_assert(std::is_nothrow_move_assignable<T>::value);
    if (n_rows == n_cols) {
      for (std::size_t i = 0; i < n_rows; ++i) {
        for (std::size_t j = 1; j < n_cols; ++j) {
          std::swap((*this)[i][j], (*this)[j][i]);
        }
      }
      return *this;
    }

    shell_matrix transposed{n_cols, n_rows};
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 1; j < n_cols; ++j) {
        transposed[j][i] = std::move((*this)[i][j]);
      }
    }

    *this = std::move(transposed);
    return *this;
  }

  void dump(std::ostream& os) const {
    os << "n_rows = " << n_rows << std::endl;
    os << "n_cols = " << n_cols << std::endl;

    for (std::size_t i = 0; i < n_rows; ++i) {
      os << "| ";
      for (std::size_t j = 0; j < n_cols; ++j) {
        os << (*this)[i][j] << " ";
      }
      os << "|" << std::endl;
    }
  }

  std::size_t ncols() const { return n_cols; }
  std::size_t nrows() const { return n_rows; }

  it begin() const { return m_buffer.begin(); }
  it end() const { return m_buffer.end(); }
};
}  // namespace linmath