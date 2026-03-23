#include <c2py/c2py.hpp>
#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
#include <typeindex>

using namespace triqs::arrays;
using namespace triqs::gfs;

namespace triqs::gfs {
  template <typename Gv> class gf_proxy {
    Gv gv;

    public:
    gf_proxy(Gv gv) : gv(gv) {}
    // Declarations only — definitions provided after wrap.cxx
    // so that the wrapper TU does not instantiate the call bodies
    // (which trigger deep mesh::evaluate chains).
    template <typename ReturnType, typename U1> ReturnType call(U1 &&x1);
    template <typename ReturnType, typename U1, typename U2> ReturnType call(U1 &&x1, U2 &&x2);
  };
} // namespace triqs::gfs

namespace c2py_module {

  // using CallProxyImFreq_0 = triqs::gfs::gf_proxy<gf_view<imfreq, scalar_valued>>;
  // using CallProxyImTime_0 = triqs::gfs::gf_proxy<gf_view<imtime, scalar_valued>>;
} // namespace c2py_module

// The inst.cpp has two parts:
// 1. using aliases (lines 1-215) — needed by wrap.cxx
// 2. extern template declarations (lines 216-1660) — suppress instantiation
#include "wrapped_aux.inst.cpp"

// The generated wrapper code — compiled with call() declared but not defined,
// so no mesh::evaluate instantiation chains occur.
#include "wrapped_aux_c2py.wrap.cxx"

// NOW provide the call definitions, after wrap.cxx has been compiled.
namespace triqs::gfs {
  template <typename Gv> template <typename ReturnType, typename U1> ReturnType gf_proxy<Gv>::call(U1 &&x1) {
    return ReturnType{gv(std::forward<U1>(x1))};
  }

  template <typename Gv> template <typename ReturnType, typename U1, typename U2> ReturnType gf_proxy<Gv>::call(U1 &&x1, U2 &&x2) {
    return ReturnType{gv(std::forward<U1>(x1), std::forward<U2>(x2))};
  }
} // namespace triqs::gfs
