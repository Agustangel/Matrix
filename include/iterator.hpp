#pragma once

#include <concepts>
#include <iterator>

namespace iterator {
template <typename T>
class myIterator {
  T* ptr = nullptr;

  myIterator(T* ptr_ = nullptr) : ptr{ptr_} {}

  T& operator*() { return *ptr; }
  const T& operator*() const { return *ptr; }

  T* operator->() { return ptr; }
  const T* operator->() const { return ptr; }

  myIterator& operator++() {
    ++ptr;
    return *this;
  }
  myIterator operator++(int) {
    myIterator tmp{*this};
    ++(*this);
    return tmp;
  }
};
}  // namespace iterator