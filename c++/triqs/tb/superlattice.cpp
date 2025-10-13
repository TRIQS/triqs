#include "./superlattice.hpp"
// #include <fmt/core.h>
// #include <fmt/ranges.h>
// #include <fmt/ostream.h>
#include <itertools/itertools.hpp>
#include <map>

constexpr auto _all_ = nda::range::all;
namespace triqs::tb {

  //superlattice::superlattice(nda::array_view<long, 2> sl_units, std::vector<std::array<long, 3>> cluster_pts)
  superlattice::superlattice(nda::array<long, 2> sl_units, nda::array<long, 2> cluster_pts) : units(sl_units), cluster_points(cluster_pts) {
    std::cout << n_cluster_sites() << " cluster points in superlattice.\n";
    if (n_cluster_sites() == 0) throw std::runtime_error{"Superlattice must have at least one cluster point."};
    TRIQS_ASSERT((units.shape() == std::array{dim(), dim()}));
    TRIQS_ASSERT(cluster_pts.extent(1) == dim());
    transfo_SL_L = nda::matrix<double>{transpose(units)};
    // Ensure non-empty cluster points
    if (abs(nda::determinant(transfo_SL_L)) != n_cluster_sites())
      throw std::runtime_error{"Mismatch between det(superlattice vectors) and number of cluster points!"};
    transfo_SL_L = nda::inverse(transfo_SL_L);
  }

  // ------------------------------------------------

  std::optional<nda::vector<long>> superlattice::L_to_SL(nda::vector<long> const &r_l, double epsilon) const {
    TRIQS_ASSERT(r_l.size() == dim());
    auto R1                     = nda::vector<double>(r_l);
    auto R                      = transfo_SL_L * r_l;
    nda::vector<long> R_rounded = nda::map([](double x) { return std::round(x); })(R); // round each element to nearest integer
    // fmt::print("r = {}, R = {}, R_rounded = {}, i = {}, j = {}\n", r, R, R_rounded, i, j);
    if (max_element(abs(R - R_rounded)) < epsilon) {
      return R_rounded; // Return the rounded vector if it is close enough to an integer vector
    } else
      return {};
  }

  // --------------------------------------------------------------
  // impl detail. Move to nda::stdutil ?
  template <int D, typename V> std::array<long, D> make_std_array_from_vector(V const &vec) {
    if (vec.size() != D) throw std::runtime_error("make_std_array_from_vector. Vector size does not match the array size.");
    std::array<long, D> arr{};
    for (int i = 0; i < D; ++i) arr[i] = vec[i];
    return arr;
  }
  // ----------------------------------------------------

  template <int kdim> fourier_polynomial<2, kdim> fold(superlattice const &sl, fourier_polynomial<2, kdim> const &fp) {
    using itertools::enumerate;
    if (sl.dim() != kdim) throw std::runtime_error("Superlattice dimension does not match the Fourier polynomial dimension.");

    auto n_orb_L   = fp.get_coefficients()[0].extent(0); // number of orbitals in the lattice
    auto n_orb_SL  = n_orb_L * sl.n_cluster_sites();     // number of orbitals in the superlattice
    auto orb_range = [&](long a) { return nda::range(a * n_orb_L, (a + 1) * n_orb_L); };

    // create containers for the new R vectors and coefficients
    std::vector<std::array<long, kdim>> SL_R_list;
    std::map<std::array<long, kdim>, long> SL_R_idx;
    std::vector<nda::array<dcomplex, 2>> SL_coeff_list;

    auto const &cpts = sl.get_cluster_pts();
    for (auto i : nda::range(sl.n_cluster_sites()))
      for (auto j : nda::range(sl.n_cluster_sites())) {
        auto ai = cpts(i, _all_);
        auto aj = cpts(j, _all_);
        //  for (auto &&[i, ai] : enumerate(sl.get_cluster_pts()))
        //   for (auto &&[j, aj] : enumerate(sl.get_cluster_pts()))
        for (auto &&[r_idx, r] : enumerate(fp.get_R_list())) {
          //fmt::print("       r = {}, i = {}, j = {}\n", r, i, j);
          auto r1 = nda::vector<long>(kdim);
          for (int u = 0; u < kdim; ++u) r1[u] = r[u] + ai[u] - aj[u];               // r1 = r + ai - aj
          auto R_rounded = sl.L_to_SL(r1);                                           // transform r + ai - aj to superlattice coordinates and round it
          if (R_rounded) {                                                           // if the rounding was successful
            auto R_rounded_2  = make_std_array_from_vector<kdim>(R_rounded.value()); // convert to std::array<long, kdim>
            auto [it, is_new] = SL_R_idx.emplace(R_rounded_2, SL_R_list.size());     // store the R_sl if not already seen
            if (is_new) {
              SL_R_list.push_back(R_rounded_2);
              SL_coeff_list.emplace_back(nda::zeros<dcomplex>(n_orb_SL, n_orb_SL));
            }
            SL_coeff_list[it->second](orb_range(i), orb_range(j)) += fp.get_coefficients()[r_idx];
          }
        }
      }
    return {std::move(SL_R_list), std::move(SL_coeff_list)};
  }

  // --------------------------------------------------

  template fourier_polynomial<2, 3> fold(superlattice const &sl, fourier_polynomial<2, 3> const &fp);
  template fourier_polynomial<2, 2> fold(superlattice const &sl, fourier_polynomial<2, 2> const &fp);
  template fourier_polynomial<2, 1> fold(superlattice const &sl, fourier_polynomial<2, 1> const &fp);

} // namespace triqs::tb
