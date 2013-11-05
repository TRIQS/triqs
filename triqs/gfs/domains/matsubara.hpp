/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#ifndef TRIQS_GF_DOMAIN_MATSUBARA_H
#define TRIQS_GF_DOMAIN_MATSUBARA_H
#include "../tools.hpp"
#include "../meshes/mesh_tools.hpp"

namespace triqs {
namespace gfs {

 // --------------- One matsubara frequency, with its arithmetics -------------------------
 // all operations are done by casting to complex, except addition and substraction of matsubara_freq

 struct matsubara_freq :  public arith_ops_by_cast<matsubara_freq, std::complex<double>>  {
  int n;
  double beta;
  statistic_enum statistic;
  matsubara_freq(int const &n_, double beta_, statistic_enum stat_) : n(n_), beta(beta_), statistic(stat_) {}
  using cast_t = std::complex<double>;
  operator cast_t() const {
   return {0, M_PI * (2 * n + (statistic == Fermion ? 1 : 0)) / beta};
  }
 };

  inline matsubara_freq operator+(matsubara_freq const &x, matsubara_freq const &y) {
  return {x.n + y.n + (x.statistic & y.statistic), x.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
 }

 inline matsubara_freq operator-(matsubara_freq const &x, matsubara_freq const &y) {
  // std::cout  << x.n - y.n - (~x.statistic & y.statistic)<< std::endl;
  return {x.n - y.n - (~x.statistic & y.statistic), x.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
 }

 /*
#define IMPL_OP(OP)                                                                                                              \
 template <typename Y> auto operator OP(matsubara_freq const &x, Y &&y)->decltype(std::complex<double>{}) {                      \
  return std::complex<double>(x) OP std::forward<Y>(y);                                                                          \
 }                                                                                                                               \
 template <typename Y>                                                                                                           \
 auto operator OP(Y &&y, matsubara_freq const &x)->decltype(std::forward<Y>(y) OP std::complex<double>{}) {                      \
  return std::forward<Y>(y) OP std::complex<double>(x);                                                                          \
 }
 IMPL_OP(+);
 IMPL_OP(-);
 IMPL_OP(*);
 IMPL_OP(/ );
#undef IMPL_OP
*/

/*
 * template <typename Y> auto operator OP(matsubara_freq const &x, Y &&y)->decltype(std::complex<double>{}) {                        \
  return std::complex<double>(x) OP std::forward<Y>(y);                                                                          \
 }                                                                                                                               \
 template <typename Y>                                                                                                           \
 auto operator OP(Y &&y, matsubara_freq const &x)                                                                                \
     ->TYPE_DISABLE_IF(decltype(std::forward<Y>(y) OP std::complex<double>{}),                                                   \
                       std::is_same<typename std::remove_cv<typename std::remove_reference<Y>::type>::type, matsubara_freq>) {   \
  return std::forward<Y>(y) OP std::complex<double>(x);                                                                          \
 }
*/

 //---------------------------------------------------------------------------------------------------------
 /// The domain
 template <bool IsFreq> struct matsubara_domain {
  using point_t = typename std::conditional<IsFreq, std::complex<double>, double>::type;
  double beta;
  statistic_enum statistic;
  matsubara_domain() = default;
  matsubara_domain(double Beta, statistic_enum s) : beta(Beta), statistic(s) {
   if (beta < 0) TRIQS_RUNTIME_ERROR << "Matsubara domain construction :  beta <0 : beta =" << beta << "\n";
  }
  matsubara_domain(matsubara_domain const &) = default;
  matsubara_domain(matsubara_domain<!IsFreq> const &x) : beta(x.beta), statistic(x.statistic) {}
  bool operator==(matsubara_domain const &D) const { return ((std::abs(beta - D.beta) < 1.e-15) && (statistic == D.statistic)); }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, matsubara_domain const &d) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "beta", d.beta);
   h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, matsubara_domain &d) {
   h5::group gr = fg.open_group(subgroup_name);
   double beta;
   std::string statistic;
   h5_read(gr, "beta", beta);
   h5_read(gr, "statistic", statistic);
   d = matsubara_domain(beta, (statistic == "F" ? Fermion : Boson));
  }

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &boost::serialization::make_nvp("beta", beta);
   ar &boost::serialization::make_nvp("statistic", statistic);
  }
 };
}
}
#endif

