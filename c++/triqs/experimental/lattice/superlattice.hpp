#pragma once

#include "./fourier_polynomial.hpp"
#include "../../utility/macros.hpp"

#include <nda/nda.hpp>

#include <optional>

namespace triqs::experimental::lattice {

  /**
   * @addtogroup triqs-experimental-lattice
   * @{
   */

  // --------------------------------------------------
  /**
   * @brief Representation of a superlattice built on top of an underlying Bravais lattice.
   *
   * @details A superlattice is defined by its unit vectors, expressed in the coordinates of the underlying lattice, and
   * a set of cluster points that enumerate the underlying-lattice sites contained in one superlattice unit cell. It
   * stores the transformation matrix from lattice to superlattice coordinates and is used to fold lattice functions
   * onto the superlattice.
   */
  class superlattice {
    nda::matrix<long> units;            // Unit vector of the superlattice units[a, _] = unit vector a
    nda::array<long, 2> cluster_points; // Cluster: set of points in original lattice coordinate
    //  NB : in d = 2, we don't use the third component.
    nda::matrix<double> transfo_SL_L; // Transformation matrix SL -> L: = inverse(transpose(units))
    long _dim = units.extent(0);      // Dimension of the superlattice

    public:
    /**
     * @brief Construct a superlattice from its unit vectors and cluster points.
     *
     * @param sl_units Unit vectors of the superlattice of shape `(2, 2)` or `(3, 3)`, where `sl_units[a, :]` is the
     * \f$ a \f$-th unit vector in the coordinates of the underlying lattice.
     * @param cluster_pts Cluster points, i.e. the underlying-lattice sites of one superlattice unit cell, given in
     * lattice coordinates.
     */
    superlattice(nda::array<long, 2> sl_units, nda::array<long, 2> cluster_pts);

    /**
     * @brief Get the unit vectors of the superlattice.
     *
     * @return Const reference to the matrix of superlattice unit vectors, where row \f$ a \f$ is the \f$ a \f$-th unit
     * vector in lattice coordinates.
     */
    C2PY_IGNORE auto const &get_sl_units() const { return units; }

    /**
     * @brief Get the cluster points of the superlattice.
     *
     * @return Const reference to the set of cluster points in lattice coordinates.
     */
    C2PY_IGNORE auto const &get_cluster_pts() const { return cluster_points; }

    /**
     * @brief Get the dimension of the superlattice.
     *
     * @return Dimension of the superlattice, either 2 or 3.
     */
    long dim() const { return _dim; }

    /**
     * @brief Get the number of cluster sites in one superlattice unit cell.
     *
     * @return Number of cluster sites.
     */
    long n_cluster_sites() const { return cluster_points.extent(0); }

    /**
     * @brief Transform a point from lattice coordinates to superlattice coordinates.
     *
     * @param r_l Point in lattice coordinates.
     * @param epsilon Tolerance used to decide whether the transformed point lies on the superlattice.
     * @return Point in superlattice coordinates if `r_l` lies on the superlattice, otherwise an empty optional.
     */
    C2PY_IGNORE std::optional<nda::vector<long>> L_to_SL(nda::vector<long> const &r_l, double epsilon = 1.e-12) const;
  };

  // --------------------------------------------------

  /**
   * @brief Fold a Fourier polynomial defined on a lattice onto a superlattice.
   *
   * @tparam kdim Dimension of the k-vectors, either 2 or 3.
   * @param sl Superlattice onto which the Fourier polynomial is folded.
   * @param fp Fourier polynomial defined on the underlying lattice.
   * @return Fourier polynomial folded onto the superlattice.
   */
  template <int kdim> fourier_polynomial<2, kdim> fold(superlattice const &sl, fourier_polynomial<2, kdim> const &fp);

  /** @} */

} // namespace triqs::experimental::lattice