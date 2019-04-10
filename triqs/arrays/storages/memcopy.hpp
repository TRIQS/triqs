#pragma once

namespace nda::mem {

  template <class S> constexpr bool is_scalar_or_pod() {
    return std::is_arithmetic<S>::value || triqs::is_complex<S>::value || std::is_pod<S>::value;
  }

  // ---------------------------

  template <typename T1, typename T2> void memcopy(T1 * p1, const T2 * p2, size_t size) {

    constexpr bool use_memcpy = is_scalar_or_pod<T1>() and std::is_same_v<T1, T2>;

    if constexpr (use_memcpy) {
      if (std::abs(p2 - p1) > size) { // guard against aliasing of data
        memcpy(p1, p2, size * sizeof(T1));
        return;
      }
      for (size_t i = 0; i < size; ++i) p1[i] = p2[i];
    }
  }

} // namespace nda::mem
