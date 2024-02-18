#pragma once

#include "vector.hpp"
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

namespace linmath {

template <typename T> class shell_matrix {
private:
  std::size_t n_rows = 0;
  std::size_t n_cols = 0;

  containers::vector<T> m_buffer;

public:
};
} // namespace linmath