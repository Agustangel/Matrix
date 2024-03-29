#pragma once

#include <algorithm>
#include <climits>
#include <concepts>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include "iterator.hpp"

namespace containers {
static constexpr unsigned BOOST_ARCH_WORD_BITS = 32;

template <typename T>
class vector {
  T* buf_begin_ptr = nullptr;
  T* buf_end_ptr = nullptr;
  T* buf_capacity_ptr = nullptr;

  static constexpr std::size_t default_capacity = 8;

 public:
  using it = iterator::myIterator<T>;

  vector()
      : buf_begin_ptr{static_cast<T*>(
            ::operator new(sizeof(T) * default_capacity))},
        buf_capacity_ptr{buf_begin_ptr + default_capacity},
        buf_end_ptr{buf_begin_ptr} {}

  explicit vector(std::size_t count, T val = T{}) {
    vector tmp{};
    tmp.reserve(count);
    tmp.fill_n(count, val);
    *this = std::move(tmp);
  }

  ~vector() {
    std::destroy(buf_begin_ptr, buf_end_ptr);
    buf_end_ptr = buf_begin_ptr;
    ::operator delete(buf_begin_ptr);
  }

  vector(vector&& rhs) noexcept {
    std::swap(buf_begin_ptr, rhs.buf_begin_ptr);
    std::swap(buf_end_ptr, rhs.buf_end_ptr);
    std::swap(buf_capacity_ptr, rhs.buf_capacity_ptr);
  }

  vector(vector& rhs) {
    vector tmp{};
    tmp.reserve(rhs.capacity());

    std::size_t sz = rhs.size();
    if constexpr (std::is_trivially_copyable<T>::value)
      std::memcpy(tmp.buf_begin_ptr, rhs.buf_begin_ptr, sz * sizeof(T));
    else
      std::uninitialized_copy(rhs.buf_begin_ptr, rhs.buf_end_ptr,
                              tmp.buf_begin_ptr);
    tmp.buf_end_ptr = tmp.buf_begin_ptr + sz;

    *this = std::move(tmp);
  }

  vector& operator=(vector&& rhs) noexcept {
    if (this == std::addressof(rhs))
      return *this;
    std::swap(buf_begin_ptr, rhs.buf_begin_ptr);
    std::swap(buf_end_ptr, rhs.buf_end_ptr);
    std::swap(buf_capacity_ptr, rhs.buf_capacity_ptr);
    return *this;
  }

  vector& operator=(const vector& rhs) {
    if (this == std::addressof(rhs))
      return *this;
    vector tmp{rhs};
    std::swap(*this, tmp);
    return *this;
  }

  T& operator[](std::size_t i) { return *(buf_begin_ptr + i); }
  const T& operator[](std::size_t i) const { return *(buf_begin_ptr + i); }

  void fill_n(std::size_t count, const T& value) {
    if (count <= 0)
      return;
    it frst = (*this).begin();
    for (std::size_t i = 0; i < count; ++i)
      *frst++ = value;
    buf_end_ptr = buf_begin_ptr + count;
  }

  void reserve(std::size_t cap) {
    if (cap <= capacity())
      return;

    T* tmp_buf = static_cast<T*>(::operator new(sizeof(T) * cap));
    auto deleter = [](T* ptr) {
      ::operator delete(ptr);
    };
    std::unique_ptr<T, decltype(deleter)> raii_buf(tmp_buf, deleter);

    const std::size_t sz = size();
    std::memcpy(raii_buf.get(), buf_begin_ptr, sz * sizeof(T));

    ::operator delete(buf_begin_ptr);
    buf_begin_ptr = raii_buf.release();
    buf_end_ptr = buf_begin_ptr + sz;
    buf_capacity_ptr = buf_begin_ptr + cap;
  }

  void reserve_if_neccessary() {
    if (buf_capacity_ptr - buf_end_ptr > 0)
      return;
    reserve(amortized(capacity()));
  }

  std::size_t amortized(std::size_t sz) const {
    return std::size_t{1} << (BOOST_ARCH_WORD_BITS - __builtin_clz(sz));
  }

  void push_back(T&& value) {
    reserve_if_neccessary();
    new (buf_end_ptr++) T{std::move(value)};
  }

  void push_back(const T& value) {
    reserve_if_neccessary();
    new (buf_end_ptr++) T{value};  // new gives a strict guarantee
  }

  void pop() noexcept { std::destroy_at(--buf_end_ptr); }

  T& front() { return *buf_begin_ptr; }
  const T& front() const { return *buf_begin_ptr; }

  T& top() { return *(buf_end_ptr - 1); }
  const T& top() const { return *(buf_end_ptr - 1); }

  void clear() noexcept {
    std::destroy(buf_begin_ptr, buf_end_ptr);
    buf_end_ptr = buf_begin_ptr;
  }

  void dump(std::ostream& os) const {
    std::size_t sz = size();
    std::size_t cap = capacity();
    os << "size = " << sz << std::endl;
    os << "capacity = " << cap << std::endl;
    os << "| ";
    for (std::size_t i = 0; i < sz; ++i)
      os << (*this)[i] << " ";
    os << "|" << std::endl;
  }

  std::size_t size() const noexcept { return buf_end_ptr - buf_begin_ptr; }
  std::size_t capacity() const noexcept {
    return buf_capacity_ptr - buf_begin_ptr;
  }

  T* data() { return buf_begin_ptr; }
  const T* data() const { return buf_begin_ptr; }

  bool empty() const noexcept { return (size() == 0); }

  it begin() const { return it{buf_begin_ptr}; }
  it end() const { return it{buf_end_ptr}; }
};
}  // namespace containers
