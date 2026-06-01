#include <c2py/c2py.hpp>
#include <cppdlr/dlr_kernels.hpp>
#include <nda/c2py/converters.hpp>
#include <triqs/c2py_converters/lattice.hpp>
#include <triqs/c2py_converters/mesh.hpp>
#include <triqs/lattice/bravais_lattice.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include <triqs/mesh/adjoint.hpp>
#include <triqs/mesh/brzone.hpp>
#include <triqs/mesh/cyclat.hpp>
#include <triqs/mesh/chebyshev.hpp>
#include <triqs/mesh/dlr_imfreq.hpp>
#include <triqs/mesh/dlr_imtime.hpp>
#include <triqs/mesh/dlr.hpp>
#include <triqs/mesh/dlr2d.hpp>
#include <triqs/mesh/dlr2d_imfreq.hpp>
#include <triqs/mesh/imfreq.hpp>
#include <triqs/mesh/imtime.hpp>
#include <triqs/mesh/legendre.hpp>
#include <triqs/mesh/refreq.hpp>
#include <triqs/mesh/refreq_log.hpp>
#include <triqs/mesh/refreq_pts.hpp>
#include <triqs/mesh/retime.hpp>
#include <triqs/mesh/utils.hpp>

template <> constexpr bool c2py::is_wrapped<triqs::lattice::bravais_lattice>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::bravais_lattice> = "lattice_tools.BravaisLattice";
template <> constexpr bool c2py::is_wrapped<triqs::lattice::brillouin_zone>      = true;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::brillouin_zone>  = "lattice_tools.BrillouinZone";

namespace triqs::mesh {

  // legendre
  template C2PY_WRAP_AS_METHOD legendre copy(legendre const &);
  template C2PY_WRAP_AS_METHOD void copy_from(legendre &, legendre const &);

  // imfreq
  template C2PY_WRAP_AS_METHOD auto values(imfreq const &);
  template C2PY_WRAP_AS_METHOD imfreq copy(imfreq const &);
  template C2PY_WRAP_AS_METHOD void copy_from(imfreq &, imfreq const &);

  // dlr_imfreq
  template C2PY_WRAP_AS_METHOD auto values(dlr_imfreq const &);
  template C2PY_WRAP_AS_METHOD dlr_imfreq copy(dlr_imfreq const &);
  template C2PY_WRAP_AS_METHOD void copy_from(dlr_imfreq &, dlr_imfreq const &);
  template dlr_imfreq::dlr_imfreq(dlr_imtime const &);
  template dlr_imfreq::dlr_imfreq(dlr const &);

  // dlr_imtime
  template C2PY_WRAP_AS_METHOD auto values(dlr_imtime const &);
  template C2PY_WRAP_AS_METHOD dlr_imtime copy(dlr_imtime const &);
  template C2PY_WRAP_AS_METHOD void copy_from(dlr_imtime &, dlr_imtime const &);
  template dlr_imtime::dlr_imtime(dlr_imfreq const &);
  template dlr_imtime::dlr_imtime(dlr const &);

  // dlr
  template C2PY_WRAP_AS_METHOD auto values(dlr const &);
  template C2PY_WRAP_AS_METHOD dlr copy(dlr const &);
  template C2PY_WRAP_AS_METHOD void copy_from(dlr &, dlr const &);
  template dlr::dlr(dlr_imtime const &);
  template dlr::dlr(dlr_imfreq const &);

  // dlr2d
  template C2PY_WRAP_AS_METHOD dlr2d copy(dlr2d const &);
  template C2PY_WRAP_AS_METHOD void copy_from(dlr2d &, dlr2d const &);

  // dlr2d_imfreq
  template C2PY_WRAP_AS_METHOD dlr2d_imfreq copy(dlr2d_imfreq const &);
  template C2PY_WRAP_AS_METHOD void copy_from(dlr2d_imfreq &, dlr2d_imfreq const &);

  // imtime
  template C2PY_WRAP_AS_METHOD auto values(imtime const &);
  template C2PY_WRAP_AS_METHOD imtime copy(imtime const &);
  template C2PY_WRAP_AS_METHOD void copy_from(imtime &, imtime const &);

  // retime
  template C2PY_WRAP_AS_METHOD auto values(retime const &);
  template C2PY_WRAP_AS_METHOD retime copy(retime const &);
  template C2PY_WRAP_AS_METHOD void copy_from(retime &, retime const &);

  // refreq
  template C2PY_WRAP_AS_METHOD auto values(refreq const &);
  template C2PY_WRAP_AS_METHOD refreq copy(refreq const &);
  template C2PY_WRAP_AS_METHOD void copy_from(refreq &, refreq const &);

  // refreq_log
  template C2PY_WRAP_AS_METHOD auto values(refreq_log const &);
  template C2PY_WRAP_AS_METHOD refreq_log copy(refreq_log const &);
  template C2PY_WRAP_AS_METHOD void copy_from(refreq_log &, refreq_log const &);

  // refreq_pts
  template C2PY_WRAP_AS_METHOD auto values(refreq_pts const &);
  template C2PY_WRAP_AS_METHOD refreq_pts copy(refreq_pts const &);
  template C2PY_WRAP_AS_METHOD void copy_from(refreq_pts &, refreq_pts const &);

  // chebyshev
  template C2PY_WRAP_AS_METHOD auto values(chebyshev const &);
  template C2PY_WRAP_AS_METHOD chebyshev copy(chebyshev const &);
  template C2PY_WRAP_AS_METHOD void copy_from(chebyshev &, chebyshev const &);

  // brzone
  template C2PY_WRAP_AS_METHOD auto values(brzone const &);
  template C2PY_WRAP_AS_METHOD brzone copy(brzone const &);
  template C2PY_WRAP_AS_METHOD void copy_from(brzone &, brzone const &);
  template triqs::mesh::brzone::index_t brzone::closest_index(nda::vector_const_view<double> const &) const;

  // cyclat
  template C2PY_WRAP_AS_METHOD auto values(cyclat const &);
  template C2PY_WRAP_AS_METHOD cyclat copy(cyclat const &);
  template C2PY_WRAP_AS_METHOD void copy_from(cyclat &, cyclat const &);

} // namespace triqs::mesh

#include "meshes.wrap.cxx"
