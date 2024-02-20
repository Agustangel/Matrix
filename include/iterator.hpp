#pragma once

#include <concepts>
#include <iterator>

namespace iterator {
template <typename T>
class myIterator {
 private:
  T* ptr = nullptr;

 public:
  myIterator(T* ptr_ = nullptr) : ptr{ptr_} {}

  T& operator*() { return *ptr; }
  const T& operator*() const { return *ptr; }

  T* operator->() { return ptr; }
  const T* operator->() const { return ptr; }

  T& operator[](std::ptrdiff_t i) { return *(*this + i); }
  const T& operator[](std::ptrdiff_t i) const { return *(*this + i); }

  myIterator& operator++() {
    ++ptr;
    return *this;
  }
  myIterator operator++(int) {
    myIterator tmp{*this};
    ++(*this);
    return tmp;
  }
  myIterator& operator--() {
    --ptr;
    return *this;
  }
  myIterator operator--(int) {
    myIterator tmp{*this};
    --(*this);
    return tmp;
  }

  myIterator& operator+=(std::ptrdiff_t i) {
    ptr += i;
    return *this;
  }
  myIterator& operator-=(std::ptrdiff_t i) {
    ptr -= i;
    return *this;
  }

  myIterator operator+(std::ptrdiff_t i) {
    myIterator tmp{*this};
    tmp += i;
    return tmp;
  }
  myIterator operator-(std::ptrdiff_t i) {
    myIterator tmp{*this};
    tmp -= i;
    return tmp;
  }

  auto operator<=>(const myIterator& rhs) const = default;
};
}  // namespace iterator