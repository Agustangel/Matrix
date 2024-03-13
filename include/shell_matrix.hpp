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

// Precision, which be used for floating point comparisons
template <typename T>
struct default_precision {
  static constexpr T prec = 1.0e-6f;
};

template <typename T>
class shell_matrix {
  std::size_t n_rows = 0;
  std::size_t n_cols = 0;

  containers::vector<T> m_buffer;

 public:
  using it = iterator::myIterator<T>;

  shell_matrix(std::size_t rows, std::size_t cols, T val = T{})
      : n_rows{rows}, n_cols{cols}, m_buffer{rows * cols, val} {}

  template <std::input_iterator it>
  shell_matrix(std::size_t rows, std::size_t cols, it frst, it lst)
      : shell_matrix{rows, cols} {
    std::size_t count = rows * cols;
    std::copy_if(frst, lst, m_buffer.begin(),
                 [&count](const auto&) { return count && count--; });
  }

  shell_matrix(std::size_t rows, std::size_t cols,
               std::initializer_list<T> list)
      : shell_matrix{rows, cols, list.begin(), list.end()} {}

  static shell_matrix zero(std::size_t rows, std::size_t cols) {
    return shell_matrix{rows, cols};
  }

  static shell_matrix identity(std::size_t sz) {
    shell_matrix ret{sz, sz};
    it leap = ret.begin();
    for (std::size_t i = 0; i != sz; ++i, leap += sz + 1)
      *leap = 1;
    return ret;
  }

  static shell_matrix diag(std::size_t sz, it start, it finish) {
    shell_matrix ret{sz, sz};
    for (std::size_t i = 0; (i != sz) && (start != finish); ++i, ++start) {
      ret[i][i] = *start;
    }
    return ret;
  }

 private:
  class proxy_row {
   private:
    T* row_ptr = nullptr;
    T* row_end_ptr = nullptr;

   public:
    proxy_row() = default;
    proxy_row(T* begin_ptr, std::size_t cols)
        : row_ptr{begin_ptr}, row_end_ptr{row_ptr + cols} {}

    T& operator[](std::size_t idx) { return row_ptr[idx]; }
    const T& operator[](std::size_t idx) const { return row_ptr[idx]; }

    it begin() const { return it{row_ptr}; }
    it end() const { return it{row_end_ptr}; }

    std::size_t size() const { return row_ptr - row_end_ptr; }
  };

  class const_proxy_row {
   private:
    const T* row_ptr;
    const T* row_end_ptr;

   public:
    const_proxy_row() = default;
    const_proxy_row(const T* begin_ptr, std::size_t cols)
        : row_ptr{begin_ptr}, row_end_ptr{row_ptr + cols} {}

    const T& operator[](std::size_t idx) const { return row_ptr[idx]; }

    it begin() const { return it{row_ptr}; }
    it end() const { return it{row_end_ptr}; }

    std::size_t size() const { return row_ptr - row_end_ptr; }
  };

 public:
  proxy_row operator[](unsigned idx) {
    return proxy_row{&m_buffer[idx * n_cols], n_cols};
  }
  const_proxy_row operator[](unsigned idx) const {
    return proxy_row{&m_buffer[idx * n_cols], n_cols};
  }

  shell_matrix& operator+=(const shell_matrix& rhs) {
    if ((n_rows != rhs.n_rows) || (n_cols != rhs.n_cols))
      throw std::runtime_error("Unsuitable matrix sizes");

    shell_matrix tmp{n_rows, n_cols, begin(), end()};
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 0; j < n_cols; ++j)
        tmp[i][j] += rhs[i][j];
    }
    *this = std::move(tmp);
    return *this;
  }

  shell_matrix& operator-=(const shell_matrix& rhs) {
    if ((n_rows != rhs.n_rows) || (n_cols != rhs.n_cols))
      throw std::runtime_error("Unsuitable matrix sizes");

    shell_matrix tmp{n_rows, n_cols, begin(), end()};
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 0; j < n_cols; ++j)
        tmp[i][j] -= rhs[i][j];
    }
    *this = std::move(tmp);
    return *this;
  }

  shell_matrix& operator*=(const T rhs) {
    shell_matrix tmp{n_rows, n_cols, begin(), end()};
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 0; j < n_cols; ++j)
        tmp[i][j] *= rhs;
    }
    *this = std::move(tmp);
    return *this;
  }

  shell_matrix& operator/=(const T rhs) {
    if (rhs == 0)
      throw std::invalid_argument("Division by zero");

    shell_matrix tmp{n_rows, n_cols, begin(), end()};
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 0; j < n_cols; ++j)
        tmp[i][j] /= rhs;
    }
    *this = std::move(tmp);
    return *this;
  }

  // cache-friendly algorithm
  shell_matrix& operator*=(const shell_matrix& rhs) {
    if (n_cols != rhs.n_rows)
      throw std::runtime_error("Unsuitable matrix sizes");

    shell_matrix tmp{n_rows, n_cols, begin(), end()};
    for (std::size_t k = 0; k < n_cols; ++k) {
      for (std::size_t i = 0; i < n_rows; ++i) {
        auto r = (*this)[i][k];
        for (std::size_t j = 0; j < rhs.n_cols; ++j)
          tmp[i][j] = r * rhs[k][j];
      }
    }
    *this = std::move(tmp);
    return *this;
  }

 public:
  T trace() const {
    if (n_rows != n_cols)
      throw std::runtime_error("Cannot get trace of non-square matrix");
    T trace{};
    it leap = begin();
    for (std::size_t idx = 0; idx != n_cols; ++idx, leap += n_cols + 1)
      trace += *leap;
    return trace;
  }

  shell_matrix& transpose() & {
    static_assert(std::is_nothrow_move_assignable<T>::value);

    if (n_rows == n_cols) {
      for (std::size_t i = 0; i < n_rows; ++i) {
        for (std::size_t j = 1; j < n_cols; ++j)
          std::swap((*this)[i][j], (*this)[j][i]);
      }
      return *this;
    }

    shell_matrix transposed{n_cols, n_rows};
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 1; j < n_cols; ++j)
        transposed[j][i] = std::move((*this)[i][j]);
    }
    *this = std::move(transposed);
    return *this;
  }

  bool is_equal(const T& lhs, const T& rhs,
                T precession = default_precision<T>::prec) const {
    return (std::abs(lhs - rhs) <= precession);
  }

  bool equel(const shell_matrix& rhs) const {
    if ((n_rows != rhs.n_rows) || (n_cols != rhs.n_cols))
      return false;

    bool answ = true;
    T precession =
        std::is_floating_point<T>::value ? default_precision<T>::prec : 0;
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 0; j < n_cols; ++j)
        answ &= is_equal((*this)[i][j], rhs[i][j], precession);
    }
    return answ;
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
  bool square() const { return nrows() == ncols(); }

  T* data() { return m_buffer.data(); }
  const T* data() const { return m_buffer.data(); }

  it begin() const { return m_buffer.begin(); }
  it end() const { return m_buffer.end(); }
};
}  // namespace linmath