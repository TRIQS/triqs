#pragma once

#include "./fourier_polynomial.hpp"
#include "./superlattice.hpp"
#include "../../utility/macros.hpp"

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <mpi/mpi.hpp>
#include <nda/h5.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <algorithm>
#include <array>
#include <ostream>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

namespace triqs::experimental::lattice {

  /**
   * @addtogroup triqs-experimental-lattice
   * @{
   */

  /**
   * @brief Tight-binding Hamiltonian \f$ H(\mathbf{k}) \f$ on a 3D lattice.
   *
   * @details A tight-binding Hamiltonian is defined by a set of lattice vectors \f$ \mathbf{R} \f$ and the associated
   * hopping (overlap) matrices \f$ t(\mathbf{R}) \f$ between orbitals. It represents
   * \f[
   *   H(\mathbf{k}) = \sum_\mathbf{R} t(\mathbf{R}) \, e^{2 \pi i \, \mathbf{k} \cdot \mathbf{R}} \; ,
   * \f]
   * and provides access to the hopping matrices, band energies and eigenvectors at a list of k-points, comparison,
   * stream output and HDF5 serialization.
   */
  class tb_hk : public fourier_polynomial<2, 3> {

    static constexpr auto r_all = nda::range::all;

    void check_hoppings();

    public:
    /// Type used for returning eigenvectors.
    using eigenvectors_t = std::pair<nda::array<double, 2>, nda::array<dcomplex, 3>>;

    /**
     * @brief Construct a tight-binding Hamiltonian from lattice vectors and their hopping matrices.
     *
     * @param Rs List of lattice vectors \f$ \mathbf{R} \f$.
     * @param hoppings List of hopping (overlap) matrices \f$ t(\mathbf{R}) \f$, one per lattice vector.
     */
    tb_hk(std::vector<std::array<long, 3>> Rs, std::vector<nda::array<dcomplex, 2>> hoppings)
       : fourier_polynomial<2, 3>(std::move(Rs), std::move(hoppings)) {

      check_hoppings();
    };

    /// Default constructor: a single zero hopping matrix for the lattice vector at the origin.
    tb_hk() : fourier_polynomial<2, 3>({{{0, 0, 0}}}, std::vector(1, nda::array<dcomplex, 2>({{dcomplex(0.)}}))) {};

    /**
     * @brief Construct a tight-binding Hamiltonian from an existing matrix-valued Fourier polynomial.
     * @param fp Matrix-valued Fourier polynomial to wrap as a tight-binding Hamiltonian.
     */
    C2PY_IGNORE tb_hk(fourier_polynomial<2, 3> fp) : fourier_polynomial<2, 3>{std::move(fp)} {}

    // ------------------------ Accessors ----------------------------

    /**
     * @brief Get a lazy range over the hopping matrices, one per R-vector.
     * @return Range over the hopping matrices \f$ t(\mathbf{R}) \f$, one per lattice vector.
     */
    [[nodiscard]] auto hoppings() {
      return std::views::iota(0L, n_R()) | std::views::transform([this](long i) { return coeff_arr(i, nda::ellipsis{}); });
    }

    /**
     * @brief Get a lazy range over the hopping matrices, one per R-vector (const overload).
     * @return Range over the hopping matrices \f$ t(\mathbf{R}) \f$, one per lattice vector.
     */
    [[nodiscard]] auto hoppings() const {
      return std::views::iota(0L, n_R()) | std::views::transform([this](long i) { return coeff_arr(i, nda::ellipsis{}); });
    }

    /**
     * @brief Get the list of real-space lattice vectors.
     * @return List of real-space lattice vectors \f$ \mathbf{R} \f$.
     */
    // Intentionally hides the C2PY_IGNORE base accessor to return the list by value, which is what gets wrapped to Python.
    // NOLINTNEXTLINE(bugprone-derived-method-shadowing-base-method)
    [[nodiscard]] std::vector<std::array<long, 3>> get_R_list() const { return fourier_polynomial<2, 3>::get_R_list(); }

    /**
     * @brief Get a lazy range over the (R-vector, hopping-matrix) pairs.
     * @return Lazy range of tuples \f$ (\mathbf{R}, t(\mathbf{R})) \f$ pairing each lattice vector with its hopping
     * matrix.
     */
    [[nodiscard]] auto elements() { return std::views::zip(get_R_list(), hoppings()); }

    /**
     * @brief Get a lazy range over the (R-vector, hopping-matrix) pairs (const overload).
     * @return Lazy range of tuples \f$ (\mathbf{R}, t(\mathbf{R})) \f$ pairing each lattice vector with its hopping
     * matrix.
     */
    [[nodiscard]] auto elements() const { return std::views::zip(get_R_list(), hoppings()); }

    /**
     * @brief Get the number of orbitals, i.e. the dimension of the Hamiltonian matrices.
     * @return Number of orbitals.
     */
    [[nodiscard]] long n_orbitals() const { return coeff_arr.shape(1); }

    //------------------- band basis energy functions ---------------------------

    /**
     * @brief Compute the band-basis energies of \f$ H(\mathbf{k}) \f$ for a list of k-points.
     *
     * @details For each k-point the Hamiltonian is diagonalized and the resulting energies are returned in the band
     * basis:
     * \f[ 
     *   H_{k,mn} = U_{\mathbf{k},ma}^\dagger \left( \sum_j t(\mathbf{R}_j)_{ab} e^{2 \pi i \mathbf{k} \cdot 
     *   \mathbf{R}_j} \right) U_{\mathbf{k},nb} \; , 
     * \f]
     * with lattice vectors \f$ \{\mathbf{R}_j\} \f$ and associated overlap (hopping) matrices 
     * \f$ \{t(\mathbf{R}_j)_{ab}\} \f$.
     * 
     * The k-points must be given in units of the reciprocal lattice vectors (from 0 to 1).
     *
     * @param k k-points as an array of shape [nk, 3] in units of the reciprocal lattice vectors.
     * @return Band-basis energies as a real matrix of shape [nk, nbands].
     */
    nda::array<double, 2> eigenvalues(nda::array_view<double, 2> k) const;

    /**
     * @brief Compute the band-basis energies and eigenvectors of \f$ H(\mathbf{k}) \f$ for a list of k-points.
     *
     * @details For each k-point the Hamiltonian is diagonalized and both the band energies and the eigenvectors are
     * returned:
     * \f[ 
     *   H_{k,mn} = U_{\mathbf{k},ma}^\dagger \left( \sum_j t(\mathbf{R}_j)_{ab} e^{2 \pi i \mathbf{k} \cdot 
     *   \mathbf{R}_j} \right) U_{\mathbf{k},nb} \; , 
     * \f]
     * with lattice displacements \f$ \{\mathbf{R}_j\} \f$ and associated overlap (hopping) matrices 
     * \f$ \{t(\mathbf{R}_j)_{ab}\} \f$.
     * 
     * The k-points must be given in units of the reciprocal lattice vectors (from 0 to 1).
     *
     * @param k k-points as an array of shape [nk, 3] in units of the reciprocal lattice vectors.
     * @return Tuple containing the band-basis energies as a real matrix of shape [nk, nbands] and the complex
     * eigenvectors as an array of shape [nk, nbands, nOrbitals].
     */
    eigenvectors_t eigenvectors(nda::array_view<double, 2> k) const;

    // ------------------- Comparison -------------------

    /**
     * @brief Equal-to comparison operator.
     *
     * @details Two tight-binding Hamiltonians are equal if they have the same list of lattice vectors and the same
     * hopping coefficients.
     *
     * @param tb Tight-binding Hamiltonian to compare with.
     * @return `true` if both Hamiltonians are equal, `false` otherwise.
     */
    bool operator==(tb_hk const &tb) const {
      return std::ranges::equal(this->get_R_list(), tb.get_R_list()) && this->get_coeff_arr() == tb.get_coeff_arr();
    }

    // ------------------- Read / Write -------------------------------

    /**
     * @brief Write a tight-binding Hamiltonian to a `std::ostream`.
     *
     * @details Prints the number of lattice vectors and the orbital dimension, and, for small Hamiltonians (fewer than
     * 12 lattice vectors), the hopping matrix associated with each lattice vector.
     *
     * @param out `std::ostream` object.
     * @param tb Tight-binding Hamiltonian to be written.
     * @return Reference to the `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &out, tb_hk const &tb) {
      out << "tb_hk consisting of " << tb.n_R() << " lattice components (R)";
      out << " with hopping dimension (n_orbitals) " << tb.n_orbitals() << "x" << tb.n_orbitals();
      // if it's a small one, we can print it:
      if (tb.n_R() < 12) {
        out << "\n[";
        for (auto const &[R, C] : tb.elements()) out << "\n   " << R << " : " << C;
        out << " ]";
      }
      return out;
    }

    /**
     * @brief Get the HDF5 format tag.
     *
     * @return HDF5 format tag of a tight-binding Hamiltonian.
     */
    [[nodiscard]] inline static std::string hdf5_format() { return "tb_hk"; }

    /**
     * @brief Write a tight-binding Hamiltonian to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param tb Tight-binding Hamiltonian to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, tb_hk const &tb) { tb.h5_write_impl(g, name, "tb_hk"); }

    /**
     * @brief Read a tight-binding Hamiltonian from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param tb Tight-binding Hamiltonian to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, tb_hk &tb) { tb.h5_read_impl(g, name, "tb_hk"); }
  };

  /**
   * @brief Fold a tight-binding Hamiltonian onto a superlattice.
   *
   * @param sl Superlattice onto which the Hamiltonian is folded.
   * @param tb Tight-binding Hamiltonian defined on the underlying lattice.
   * @return Tight-binding Hamiltonian folded onto the superlattice.
   */
  inline tb_hk fold(superlattice const &sl, tb_hk const &tb) { return fold(sl, static_cast<fourier_polynomial<2, 3> const &>(tb)); }

  /** @} */

} // namespace triqs::experimental::lattice
