#pragma once
#include <triqs/gfs.hpp>

namespace triqs::gfs {

  template <typename Gv> class gf_proxy {
    Gv gv;

    public:
    gf_proxy(Gv gv) : gv(gv) {}
    template <typename ReturnType, typename U1> ReturnType operator()(U1 &&x1) { return ReturnType{gv(std::forward<U1>(x1))}; }
    template <typename ReturnType, typename U1, typename U2> ReturnType operator()(U1 &&x1, U2 &&x2) {
      return ReturnType{gv(std::forward<U1>(x1), std::forward<U2>(x2))};
    }
  };

} // namespace triqs::gfs
