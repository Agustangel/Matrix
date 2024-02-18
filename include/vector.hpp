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

template <typename T> class vector {
private:
  T *buf_begin_ptr = nullptr;
  T *buf_end_ptr = nullptr;
  T *buf_capacity_ptr = nullptr;

  static constexpr std::size_t default_capacity = 8;

public:
  vector()
      : buf_begin_ptr{static_cast<T *>(
            ::operator new(sizeof(T) * default_capacity))},
        buf_capacity_ptr{buf_begin_ptr + default_capacity},
        buf_end_ptr{buf_begin_ptr} {};

  void vector_expand(std::size_t cap) {
    if (cap <= capacity())
      return;

    T *tmp_buf = static_cast<T *>(::operator new(sizeof(T) * cap));
    auto deleter = [](T *ptr) { ::operator delete(ptr); };
    std::unique_ptr<T, decltype(deleter)> raii_buf(tmp_buf, deleter);

    const std::size_t sz = size();
    std::memcpy(raii_buf.get(), buf_begin_ptr, sz * sizeof(T));

    ::operator delete(buf_begin_ptr);
    buf_begin_ptr = raii_buf.release();
    buf_end_ptr = buf_begin_ptr + sz;
    buf_capacity_ptr = buf_begin_ptr + cap;
  }

  std::size_t size() const noexcept { return buf_end_ptr - buf_begin_ptr; }
  std::size_t capacity() const noexcept {
    return buf_capacity_ptr - buf_begin_ptr;
  }

  bool empty() const noexcept { return (size() == 0); }
};
} // namespace containers
