#pragma once

#include <algorithm>
#include <climits>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace containers {
static constexpr unsigned BOOST_ARCH_WORD_BITS = 32;

template <typename T>
class vector {
 private:
  T* buf_begin_ptr = nullptr;
  T* buf_end_ptr = nullptr;
  T* buf_capacity_ptr = nullptr;

  static constexpr std::size_t default_capacity = 8;

 public:
  vector()
      : buf_begin_ptr{static_cast<T*>(
            ::operator new(sizeof(T) * default_capacity))},
        buf_capacity_ptr{buf_begin_ptr + default_capacity},
        buf_end_ptr{buf_begin_ptr} {};

  explicit vector(std::size_t count, T val = T{}) {
    vector<T> tmp{};
    tmp.expand(count);
    std::ranges::copy(std::views::repeat_n(value, count),
                      std::back_inserter(tmp));

    *this = std::move(tmp);
  };

  vector(T* frst, T* lst) {
    vector<T> tmp{};
    std::copy(frst, lst, std::back_inserter(tmp));

    *this = std::move(tmp);
  };

  ~vector() {
    std::destroy(buf_begin_ptr, buf_end_ptr);
    buf_end_ptr = buf_begin_ptr;
    ::operator delete(buf_begin_ptr);
  };

  vector(vector&& rhs) noexcept {
    std::swap(buf_begin_ptr, rhs.buf_begin_ptr);
    std::swap(buf_end_ptr, rhs.buf_end_ptr);
    std::swap(buf_capacity_ptr, rhs.buf_capacity_ptr);
  }

  vector(vector& rhs) {
    vector tmp{};
    tmp.expand(rhs.capacity());

    *this = std::move(tmp);
  }

  void expand(std::size_t cap) {
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

 private:
  void expand_if_neccessary() {
    if (buf_capacity_ptr - buf_end_ptr > 0)
      return;
    expand(amortized(capacity()));
  }

 public:
  std::size_t amortized(std::size_t sz) const {
    return std::size_t{1} << (BOOST_ARCH_WORD_BITS - __builtin_clz(sz));
  }

  void push_back(T&& value) {
    expand_if_neccessary();
    new (buf_end_ptr++) T{std::move(value)};
  }

  void push_back(const T& value) {
    expand_if_neccessary();
    new (buf_end_ptr++) T{value};  // new gives a strict guarantee
  }

  void clear() noexcept { std::destroy(buf_begin_ptr, buf_end_ptr); }

  std::size_t size() const noexcept { return buf_end_ptr - buf_begin_ptr; }
  std::size_t capacity() const noexcept {
    return buf_capacity_ptr - buf_begin_ptr;
  }

  bool empty() const noexcept { return (size() == 0); }
};
}  // namespace containers
