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
  containers::vector<T*> m_m_rows_vec;

 public:
  using it = iterator::myIterator<T>;

  matrix(std::size_t rows, std::size_t cols, T val = T{})
      : m_shell_matrix{rows, cols, val} {
    reserve_m_rows_vec();
  }

  matrix(std::size_t rows, std::size_t cols, T* frst, T* lst)
      : m_shell_matrix{rows, cols, frst, lst} {
    reserve_m_rows_vec();
  }

  matrix(std::size_t rows, std::size_t cols, std::initializer_list<T> list)
      : m_shell_matrix{rows, cols, list} {
    reserve_m_rows_vec();
  }

  matrix(shell_matrix<T>&& rhs) : m_shell_matrix(std::move(rhs)) {
    reserve_m_rows_vec();
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
  void reserve_rows_vec() {
    std::size_t n_cols = ncols();
    std::size_t n_rows = nrows();

    m_rows_vec.reserve(n_rows);
    for (std::size_t idx = 0; idx != n_rows; ++idx) {
      m_rows_vec[idx] = m_shell_matrix.data() + idx * n_cols;
    }
  }

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
    return proxy_row{m_rows_vec[idx], ncols()};
  }
  const proxy_row& operator[](unsigned idx) const {
    return proxy_row{m_rows_vec[idx], ncols()};
  }

  matrix& operator*=(T value) {
    m_shell_matrix *= value;
    return *this;
  }

  matrix& operator/=(T value) {
    m_shell_matrix /= value;
    return *this;
  }

  matrix& operator+=(const matrix& rhs) {
    m_shell_matrix += rhs.m_shell_matrix;
    return *this;
  }

  matrix& operator-=(const matrix& rhs) {
    m_shell_matrix -= rhs.m_shell_matrix;
    return *this;
  }

  matrix& operator*=(const matrix& rhs) {
    m_shell_matrix *= rhs.m_shell_matrix;
    return *this;
  }

 public:
  bool equel(const matrix& rhs) {
    return m_shell_matrix.equel(rhs.m_shell_matrix);
  }

  matrix& transpose() & {
    static_assert(std::is_nothrow_move_assignable<T>::value);

    auto n_rows = nrows();
    auto n_cols = ncols();
    matrix transposed{n_rows, n_cols};
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 1; j < n_cols; ++j)
        transposed[j][i] = std::move((*this)[i][j]);
    }
    *this = std::move(transposed);
    return *this;
  }

 private:
  void swap_rows(std::size_t idx1, std::size_t idx2) {
    std::swap(m_rows_vec[idx1], m_rows_vec[idx2]);
  }

  bool is_greater(const T& lhs, const T& rhs,
                  T precession = default_precision<T>::prec) const {
    return (std::abs(lhs - rhs) > precession);
  }

  std::pair<T, std::size_t> get_greater_in_col(std::size_t col_num,
                                               std::size_t row_pos) const {
    T max = m_shell_matrix[row_pos][col_num];
    std::size_t max_pos = row_pos;
    for (std::size_t i = row_pos + 1; row_pos < nrows(); ++row_pos) {
      if (is_greater(m_shell_matrix[i][col_num], max)) {
        max = m_shell_matrix[i][col_num];
        max_pos = i;
      }
    }
    return std::make_pair<max, max_pos>;
  }

  std::optional<int> convert_to_upper_triangle()
      requires std::is_floating_point<T>::value {
    int sign = 1;
    for (std::size_t i = 0; i < ncols(); ++i) {
      auto ret = get_greater_in_col(i, i);
      if (ret.first() == T{})
        return std::nullopt;
      if (ret.second() != i) {
        swap_rows(ret.second(), i);
        sign *= -1;
      }
      for (std::size_t j = i + 1; j < nrows(); ++j) {
        auto coef = m_shell_matrix[j][i] / ret.first();
        for (std::size_t n_elem = i; n_elem < ncols(); ++n_elem)
          m_shell_matrix[j][n_elem] -= coef * m_shell_matrix[i][n_elem];
      }
    }
    return sign;
  }

 public:
  T float_determinant() const requires std::is_floating_point<T>::value {
    if (!square())
      throw std::runtime_error("Unsuitable matrix size for determinant");

    matrix tmp{*this};
    auto ret = tmp.convert_to_upper_triangle();
    if (!ret)
      return T{};

    T val = ret.value();
    for (std::size_t i = 0; i < rows(); ++i) {
      val *= tmp[i][i];
    }

    return val;
  }

  std::size_t nrows() const { return m_shell_matrix.rows(); }
  std::size_t ncols() const { return m_shell_matrix.cols(); }

  bool square() const { return nrows() == ncols(); }

  it begin() const { return m_shell_matrix.begin(); }
  it end() const { return m_shell_matrix.end(); }
};
}  // namespace linmath