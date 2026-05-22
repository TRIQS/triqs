// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2016 Igor Krivenko
// Copyright (c) 2018-2023 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell, Thomas Ayral

/**
 * @file
 * @brief Provides a tight-binding Hamiltonian class for Bravais lattices and associated utilities.
 */

#pragma once

#include "./brillouin_zone.hpp"
#include "../gfs/gf/gf.hpp"
#include "../mesh/brzone.hpp"
#include "../utility/macros.hpp"

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <nda/linalg.hpp>
#include <nda/nda.hpp>
#include <nda/stdutil/complex.hpp>

#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace triqs::lattice {

  /**
   * @addtogroup triqs-lattice
   * @{
   */

  /**
   * @brief Ordered dictionary mapping lattice displacements to hopping (overlap) matrices.
   *
   * @details Entries are stored in the order in which they are inserted. The two vectors displ_vec and
   * overlap_mat_vec are kept aligned: entry `i` describes the hopping from a reference orbital to the orbitals
   * located at displacement `displ_vec[i]` (in units of the lattice basis vectors), and `overlap_mat_vec[i]` is the
   * corresponding orbital-to-orbital overlap matrix.
   */
  struct C2PY_IGNORE hopping_dict {
    /// `std::vector` of `nda::vector<long>` displacement vectors, in units of the lattice basis vectors.
    std::vector<nda::vector<long>> displ_vec;

    /// `std::vector` of `nda::matrix<dcomplex>` overlap (hopping) matrices, one per displacement.
    std::vector<nda::matrix<dcomplex>> overlap_mat_vec;
  };

  /**
   * @brief Tight-binding Hamiltonian on a Bravais lattice with fully localised orbitals.
   *
   * @details The Hamiltonian is parametrised by a set of lattice displacements \f$ \{ \mathbf{R}_j \} \f$ (given in 
   * units of the lattice basis vectors) and the associated overlap (hopping) matrices \f$ \{ t_{\mathbf{R}_j} \} \f$ 
   * between orbitals in the unit cell. The Bloch Hamiltonian in reciprocal space is obtained by the discrete Fourier
   * transform
   * \f[
   *   h_{\mathbf{k}} = \sum_j t_{\mathbf{R}_j} \, e^{2 \pi i \, \mathbf{k} \cdot \mathbf{R}_j} \; ,
   * \f]
   * where the momentum \f$ \mathbf{k} \f$ is expressed in units of the reciprocal lattice basis vectors.
   *
   * The orbital overlap within a unit cell (the on-site block at \f$ \mathbf{R} = 0 \f$) is the identity matrix unless
   * explicitly overridden by the user-provided hoppings.
   */
  class tight_binding {

    bravais_lattice bl_;
    std::vector<nda::vector<long>> displ_vec_;
    std::vector<nda::matrix<dcomplex>> overlap_mat_vec_;

    void check_hoppings();

    public:
    /**
     * @brief Construct a tight-binding Hamiltonian on a given Bravais lattice from explicit displacement and overlap 
     * lists.
     *
     * @details The matrix structure of each overlap matrix is with respect to the orbitals in the unit cell. The
     * displacement and overlap lists must have the same length, and every overlap matrix must be square with size
     * equal to the number of orbitals in the unit cell.
     *
     * @param bl Underlying Bravais lattice.
     * @param displ_vec List of displacement vectors, in units of the lattice basis vectors.
     * @param overlap_mat_vec List of overlap (hopping) matrices, one per displacement.
     */
    tight_binding(bravais_lattice bl, std::vector<nda::vector<long>> displ_vec, std::vector<nda::matrix<dcomplex>> overlap_mat_vec);

    /**
     * @brief Construct a tight-binding Hamiltonian on a given Bravais lattice from a hopping dictionary.
     *
     * @param bl Underlying Bravais lattice.
     * @param hoppings Hopping dictionary mapping displacement vectors to their overlap matrices.
     */
    tight_binding(bravais_lattice bl, hopping_dict hoppings);

    /// Get the underlying Bravais lattice.
    bravais_lattice const &lattice() const { return bl_; }

    /// Get the list of displacement vectors, in units of the lattice basis vectors.
    auto const &displ_vec() const { return displ_vec_; }

    /// Get the list of overlap (hopping) matrices, aligned with the displacement vectors.
    auto const &overlap_mat_vec() const { return overlap_mat_vec_; }

    /**
     * @brief Transform a vector from the lattice basis to the standard basis.
     *
     * @details Equivalent to calling lattice_to_real_coordinates() on the underlying Bravais lattice.
     *
     * @param x Vector in the lattice basis.
     * @return Vector in the standard basis.
     */
    template <typename R> r_t lattice_to_real_coordinates(R const &x) const { return bl_.lattice_to_real_coordinates(x); }

    /// Number of orbitals (also the size of the Bloch Hamiltonian matrix \f$ h_{\mathbf{k}} \f$).
    long n_orbitals() const { return bl_.n_orbitals(); }

    /**
     * @brief Apply a callable to every `(displacement, overlap matrix)` pair stored in the tight-binding Hamiltonian.
     *
     * @param tb Tight-binding Hamiltonian to iterate over.
     * @param f Callable invoked as `f(R, t_R)` for each stored displacement `R` and its overlap matrix `t_R`.
     */
    template <typename F> friend void foreach (tight_binding const &tb, F f) {
      int n = static_cast<int>(tb.displ_vec_.size());
      for (int i = 0; i < n; ++i) f(tb.displ_vec_[i], tb.overlap_mat_vec_[i]);
    }

    /**
     * @brief Compute the Fourier transform for a given momentum vector (or array of momentum vectors).
     *
     * @details The Bloch Hamiltonian is given by
     * \f[
     *   h_{\mathbf{k}} = \sum_j t_{\mathbf{R}_j} \, e^{2 \pi i \, \mathbf{k} \cdot \mathbf{R}_j} \; ,
     * \f]
     * with lattice displacements \f$ \{ \mathbf{R}_j \} \f$ and associated overlap (hopping) matrices
     * \f$ \{ t_{\mathbf{R}_j} \} \f$. The momentum \f$ \mathbf{k} \f$ is expressed in units of the reciprocal lattice
     * basis vectors.
     *
     * @param k Momentum vector (or an array of momentum vectors) in units of the reciprocal lattice basis vectors.
     * @return Complex matrix \f$ h_{\mathbf{k}} \f$ (or an array of such matrices, one per input momentum).
     */
    template <typename K>
      requires(nda::ArrayOfRank<K, 1> or nda::ArrayOfRank<K, 2>)
    auto fourier(K const &k) const {
      // Make sure to account for ndim in lattice
      auto k_ndim = make_regular(k(nda::ellipsis(), range(lattice().ndim())));

      auto vals = [&](int j) {
        if constexpr (nda::ArrayOfRank<K, 1>) {
          return std::exp(2i * M_PI * nda::linalg::dot_generic(k_ndim, displ_vec_[j])) * overlap_mat_vec_[j];
        } else { // Rank==2
          auto k_mat = nda::make_matrix_view(k_ndim);
          auto exp   = [](auto d) { return std::exp(d); };
          auto exp_j = make_regular(nda::map(exp)(2i * M_PI * k_mat * displ_vec_[j]));
          return nda::linalg::outer_product(exp_j, overlap_mat_vec_[j]);
        }
      };
      auto res = make_regular(vals(0));
      for (int i = 1; i < displ_vec_.size(); ++i) res += vals(i);
      return res;
    }

    /**
     * @brief Compute the Fourier transform on a given Brillouin zone mesh.
     *
     * @param k_mesh Brillouin zone mesh on which to evaluate the Bloch Hamiltonian.
     * @return Matrix-valued Green's function defined on `k_mesh`, with its data initialised with the Fourier transform
     * \f$ h_{\mathbf{k}} \f$ at every mesh point.
     */
    inline auto fourier(mesh::brzone const &k_mesh) const {
      auto kvecs = nda::matrix<double>(k_mesh.size(), 3);
      for (auto [n, k] : itertools::enumerate(k_mesh)) { kvecs(n, range::all) = k.value(); }
      auto kvecs_rec = make_regular(kvecs * k_mesh.bz().reciprocal_matrix_inv());
      auto h_k       = gfs::gf<mesh::brzone, gfs::matrix_valued>(k_mesh, {n_orbitals(), n_orbitals()});
      h_k.data()     = fourier(kvecs_rec);
      return h_k;
    }

    /**
     * @brief Compute the Fourier transform on a regular Brillouin zone mesh with `n_l` points per dimension.
     *
     * @param n_l Number of grid-points along each reciprocal direction.
     * @return Matrix-valued Green's function defined on the regular Brillouin zone mesh, with its data initialised with
     * the Fourier transform \f$ h_{\mathbf{k}} \f$ at every mesh point.
     */
    inline auto fourier(int n_l) const {
      auto k_mesh = mesh::brzone(brillouin_zone{bl_}, n_l);
      return fourier(k_mesh);
    }

    /**
     * @brief Compute the dispersion, i.e. the eigenvalue spectrum of \f$ h_{\mathbf{k}} \f$, for a given momentum vector
     * (or array of momentum vectors).
     *
     * @param k Momentum vector (or an array of momentum vectors) in units of the reciprocal lattice basis vectors.
     * @return Real-valued array of length `n_orbitals` containing the band energies at \f$ \mathbf{k} \f$, or an array
     * of such band-energy arrays when an array of momenta is passed.
     */
    template <typename K>
    auto dispersion(K const &k) const
      requires(nda::ArrayOfRank<K, 1> or nda::ArrayOfRank<K, 2>)
    {
      if constexpr (nda::ArrayOfRank<K, 1>) {
        return nda::linalg::eigvalsh(fourier(k));
      } else { // Rank==2
        auto h_k = fourier(k);
        auto n_k = h_k.shape()[0];
        auto res = nda::array<double, 2>(n_k, n_orbitals());
        for (auto l : range(n_k)) res(l, range::all) = nda::linalg::eigvalsh(h_k(l, nda::ellipsis()));
        return res;
      }
    }

    /**
     * @brief Compute the dispersion on a given Brillouin zone mesh.
     *
     * @param k_mesh Brillouin zone mesh on which to evaluate the band energies.
     * @return Tensor-valued Green's function defined on `k_mesh`, with its data initialised with the band energies at
     * every mesh point (one real value per orbital).
     */
    inline auto dispersion(mesh::brzone const &k_mesh) const {
      auto h_k = fourier(k_mesh);
      auto e_k = gfs::gf<mesh::brzone, gfs::tensor_real_valued<1>>(k_mesh, {n_orbitals()});
      for (auto k : k_mesh) e_k[k] = nda::linalg::eigvalsh(h_k[k]);
      return e_k;
    }

    /**
     * @brief Compute the dispersion on a regular Brillouin zone mesh with `n_l` points per dimension.
     *
     * @param n_l Number of grid-points along each reciprocal direction.
     * @return Tensor-valued Green's function defined on the regular Brillouin zone mesh, with its data initialised with
     * the band energies at every mesh point.
     */
    inline auto dispersion(int n_l) const {
      auto k_mesh = mesh::brzone(brillouin_zone{bl_}, n_l);
      return dispersion(k_mesh);
    }

    // ------------------- Comparison -------------------

    /// Equal-to comparison operator. True if the underlying lattice, displacements and overlap matrices all match.
    bool operator==(tight_binding const &tb) const { return bl_ == tb.bl_ && overlap_mat_vec_ == tb.overlap_mat_vec_ && displ_vec_ == tb.displ_vec_; }

    /// Not-equal-to comparison operator (negation of operator==()).
    bool operator!=(tight_binding const &tb) const { return !(operator==(tb)); }

    // -------------------- print -------------------

    /**
     * @brief Write a tight-binding Hamiltonian to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param tb Tight-binding Hamiltonian to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, tight_binding const &tb) {
      sout << "Tight Binding Hamiltonian on " << tb.lattice() << "\nwith hoppings [";
      for (auto const &[displ, overlap_mat] : itertools::zip(tb.displ_vec(), tb.overlap_mat_vec())) sout << "\n   " << displ << " : " << overlap_mat;
      return sout << " ]";
    }

    // ------------------- HDF5 Read / Write -------------------

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "tight_binding"; }

    /**
     * @brief Write a tight-binding Hamiltonian to HDF5.
     *
     * @param fg `h5::group` to be written to.
     * @param subgroup_name Name of the subgroup.
     * @param tb Tight-binding Hamiltonian to be written.
     */
    friend void h5_write(h5::group fg, std::string subgroup_name, tight_binding const &tb) {
      auto grp = fg.create_group(subgroup_name);
      write_hdf5_format(grp, tb); // NOLINT
      h5_write(grp, "bravais_lattice", tb.bl_);
      h5_write(grp, "displ_vec", tb.displ_vec_);
      h5_write(grp, "overlap_mat_vec", tb.overlap_mat_vec_);
    }

    /**
     * @brief Construct a tight-binding Hamiltonian by reading it from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param subgroup_name Name of the subgroup.
     * @return The reconstructed tight-binding Hamiltonian.
     */
    CPP2PY_IGNORE
    static tight_binding h5_read_construct(h5::group g, std::string subgroup_name) {
      auto grp             = g.open_group(subgroup_name);
      auto bl              = h5::h5_read<bravais_lattice>(grp, "bravais_lattice");
      auto displ_vec       = h5::h5_read<std::vector<nda::vector<long>>>(grp, "displ_vec");
      auto overlap_mat_vec = h5::h5_read<std::vector<nda::matrix<dcomplex>>>(grp, "overlap_mat_vec");
      return tight_binding(bl, displ_vec, overlap_mat_vec);
    }

  }; // tight_binding

  /**
   * @brief Compute the density of states of a tight-binding Hamiltonian on a regular k-grid.
   *
   * @details The Brillouin zone is sampled by a regular grid of \f$ n_\text{kpts}^d \f$ momentum points (where \f$ d \f$
   * is the dimension of the lattice), the dispersion is diagonalised on each grid point, and the resulting band
   * energies are histogrammed into `neps` bins per orbital.
   *
   * @param TB Tight-binding Hamiltonian.
   * @param nkpts Number of k-points along each dimension.
   * @param neps Number of energy bins.
   * @return Pair `(energies, dos)`, where `energies` is a 1-D array of bin centres and `dos` is a 2-D array of shape
   * `(n_orbitals, neps)` containing one density-of-states histogram per orbital.
   */
  std::pair<nda::array<double, 1>, nda::array<double, 2>> dos(tight_binding const &TB, int nkpts, int neps);

  /**
   * @brief Compute the density of states of a tight-binding Hamiltonian on a triangular Brillouin zone patch.
   *
   * @note Only supported for 2-dimensional lattices.
   *
   * @param TB Tight-binding Hamiltonian.
   * @param triangles 2-D array of shape `(n_triangles * 3, 2)` containing the vertices of the triangular patches in the
   * Brillouin zone, three rows per triangle.
   * @param neps Number of energy bins.
   * @param ndiv Number of sub-divisions of each triangle used for the sampling.
   * @return Pair `(energies, dos)`, where `energies` is a 1-D array of bin centres and `dos` is a 1-D array of the same
   * length containing the total density of states summed over orbitals.
   */
  std::pair<nda::array<double, 1>, nda::array<double, 1>> dos_patch(tight_binding const &TB, const nda::array<double, 2> &triangles, int neps,
                                                                    int ndiv);

  /** @} */

} // namespace triqs::lattice
