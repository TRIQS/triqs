#pragma once

namespace triqs::gfs {

  template <typename V, typename T, int A> auto fourier(block_gf<V, T, A> const &g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  };

  template <typename V, typename T, int A> auto fourier(block_gf<V, T, A> &g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  };

  template <typename V, typename T, int A, bool C> auto fourier(block_gf_view<V, T, A, C> g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  };

} // namespace triqs::gfs
