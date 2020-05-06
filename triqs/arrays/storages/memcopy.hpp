#pragma once

#include <cstring>

namespace triqs::arrays::mem {

  // FIXME : This is only for assignment. Move it.
  template <typename T1, typename T2> void memcopy(T1 * p1, const T2 * p2, size_t size) {

    constexpr bool use_memcpy = std::is_trivially_copyable_v<T1> and std::is_same_v<T1, T2>;

    if constexpr (use_memcpy) {
      if (std::abs(p2 - p1) > size) { // guard against overlapping of data
	 std::memcpy(p1, p2, size * sizeof(T1));
        return;
      }
      for (size_t i = 0; i < size; ++i) p1[i] = p2[i];
    }
  }

} // namespace triqs::arrays::mem
