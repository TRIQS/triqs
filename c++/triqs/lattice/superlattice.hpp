#pragma once
#include "fourier_polynomial.hpp"
#include <nda/nda.hpp>

namespace triqs::lattice {

  namespace stdr = std::ranges;

  // --------------------------------------------------
  /**
 * @brief Representation of a superlattice over a lattice.
 *
 *  Defined by the unit vectors of the superlattice and a set of cluster points.
 * 
 */
  class superlattice {
    nda::matrix<long> units;            // Unit vector of the superlattice units[a, _] = unit vector a
    nda::array<long, 2> cluster_points; // Cluster: set of points in original lattice coordinate
    //std::vector<std::array<long, 3>> cluster_points; // Cluster: set of points in original lattice coordinate
    // NB : in d = 2, we don't use the third component.
    nda::matrix<double> transfo_SL_L; // Transformation matrix SL -> L: = inverse(transpose(units))
    long _dim = units.extent(0);      // Dimension of the superlattice

    public:
    /**
     * @brief Constructor 
     * 
     * @param sl_units Unit vectors of the superlattice of shape (2,2) or (3,3). sl_units[a, _] = unit vector a
     * @param cluster_pts Cluster defined as a set of points in original lattice coordinate. 
     */
    superlattice(nda::array<long, 2> sl_units, nda::array<long, 2> cluster_pts);
    //superlattice(nda::array_view<long, 2> sl_units, std::vector<std::array<long, 3>> cluster_pts);

    // 1 problem to wrap it : const & return [not yet implemented in c2py]
    C2PY_IGNORE auto const &get_sl_units() const { return units; }
    C2PY_IGNORE auto const &get_cluster_pts() const { return cluster_points; }

    /// Dimension of the superlattice (2 or 3)
    long dim() const { return _dim; }

    /// Number of cluster sites in the superlattice
    long n_cluster_sites() const { return long(cluster_points.extent(0)); }

    /// Transform a point from lattice coordinates to superlattice coordinates
    /// if it is on the superlattice, otherwise return empty.
    // TODO : return R + residue ?
    C2PY_IGNORE std::optional<nda::vector<long>> L_to_SL(nda::vector<long> const &r_l, double epsilon = 1.e-12) const;
  };

  // --------------------------------------------------

  /**
   * @brief Folds a fourier polynomial defined on a lattice into a superlattice.
   * 
   * @tparam kdim: 2 or 3 
   * @param sl A superlattice
   * @param fp 
   * @return  Folded fp
   */
  template <int kdim> fourier_polynomial<2, kdim> fold(superlattice const &sl, fourier_polynomial<2, kdim> const &fp);

} // namespace triqs::lattice