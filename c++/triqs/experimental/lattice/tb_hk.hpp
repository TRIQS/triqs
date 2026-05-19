#pragma once
#include <nda/nda.hpp>
#include <mpi/mpi.hpp>
#include <nda/mpi.hpp>
#include <cassert>
#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <ranges>
#include "fourier_polynomial.hpp"
#include "superlattice.hpp"
#include <nda/h5.hpp>

namespace triqs::experimental::lattice {

  class tb_hk : public fourier_polynomial<2, 3> {

    static constexpr auto r_all = nda::range::all;

    void check_hoppings();

    public:
    tb_hk(std::vector<std::array<long, 3>> Rs, std::vector<nda::array<dcomplex, 2>> hoppings)
       : fourier_polynomial<2, 3>(std::move(Rs), std::move(hoppings)) {

      check_hoppings();
    };

    /// default constructor to zero hopping for single R vector at the origin
    tb_hk() : fourier_polynomial<2, 3>({{{0, 0, 0}}}, std::vector(1, nda::array<dcomplex, 2>({{dcomplex(0.)}}))) {};

    C2PY_IGNORE tb_hk(fourier_polynomial<2, 3> fp) : fourier_polynomial<2, 3>{std::move(fp)} {}

    // ------------------------ Accessors ----------------------------

    /// Lazy range of 2D views into the packed coefficient array, one per R-vector.
    [[nodiscard]] auto hoppings() {
      return std::views::iota(0L, n_R()) | std::views::transform([this](long i) { return coeff_arr(i, nda::ellipsis{}); });
    }

    /// Lazy range of 2D views into the packed coefficient array, one per R-vector.
    [[nodiscard]] auto hoppings() const {
      return std::views::iota(0L, n_R()) | std::views::transform([this](long i) { return coeff_arr(i, nda::ellipsis{}); });
    }

    /**
     * @brief Provide an iterator of tuples of $$(R, t_{R, ab})$$
     * @return elements : tuple of (R, t_{R,ba}) pairs
     */
    [[nodiscard]] auto elements() { return std::views::zip(get_R_list(), hoppings()); }
    [[nodiscard]] auto elements() const { return std::views::zip(get_R_list(), hoppings()); }

    /**
     * @brief Provide number of orbitals (the dimension of the stored Hamiltonian)
     * @return n_orbitals
     */
    [[nodiscard]] long n_orbitals() const { return coeff_arr.shape(1); }

    //------------------- band basis energy functions ---------------------------

    /**
     * @brief Function to take a list of kpoints and return a matrix of of energies in the diagonalized (band) space.
     *
     * $$ H_{k,mn} = U_{\mathbf{k},ma}^\dagger (\sum_j t(\mathbf{R}_j)_{ab} * exp(2 \pi i * \mathbf{k} * \mathbf{R}_j) U_{\mathbf{k},nb} $$
     *
     * with lattice vectors {R_j} and associated overlap (hopping) matrices {t(\mathbf{R}_j)_{ab}}
     * k needs to be represented in units of the reciprocal lattice vectors (from 0 to 1)
     *
     * @param k k-points as an array of [nk, 3] in units of the reciprocal lattice vectors
     * @return energies as a matrix of shape [nk, nbands] of band-basis energies as real doubles
     */
    nda::array<double, 2> eigenvalues(nda::array_view<double, 2> k) const;

    /**
     * @brief Function to take a list of kpoints and return a tuple containing the band energies and the eigenvectors of H(k)
     *
     * $$ H_{k,mn} = U_{\mathbf{k},ma}^\dagger (\sum_j t(\mathbf{R}_j)_{ab} * exp(2 \pi i * \mathbf{k} * \mathbf{R}_j) U_{\mathbf{k},nb} $$
     *
     * with lattice displacements {R_j} and associated overlap (hopping) matrices {t(\mathbf{R}_j)_{ab}}
     * k needs to be represented in units of the reciprocal lattice vectors (from 0 to 1)
     *
     * @param k k-points as an array of [nk, 3] in units of the reciprocal lattice vectors
     * @return tuple containing band-basis energies as a matrix of real doubles with shape [nk, nbands] and complex eigenvectors as a matrix of shape
     * [nk,nBand,nOrbitals]
     */
    std::pair<nda::array<double, 2>, nda::array<dcomplex, 3>> eigenvectors(nda::array_view<double, 2> k) const;

    // ------------------- Comparison -------------------
    bool operator==(tb_hk const &tb) const {
      return std::ranges::equal(this->get_R_list(), tb.get_R_list()) && this->get_coeff_arr() == tb.get_coeff_arr();
    }

    // ------------------- Read / Write -------------------------------

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

    [[nodiscard]] inline static std::string hdf5_format() { return "tb_hk"; }

    friend void h5_write(h5::group g, std::string const &name, tb_hk const &tb) { tb.h5_write_impl(g, name, "tb_hk"); }

    friend void h5_read(h5::group g, std::string const &name, tb_hk &tb) { tb.h5_read_impl(g, name, "tb_hk"); }
  };

  // Superlattice folding user function
  /// @brief Fold the tight-binding Hamiltonian into a superlattice
  inline tb_hk fold(superlattice const &sl, tb_hk const &tb) { return fold(sl, static_cast<fourier_polynomial<2, 3> const &>(tb)); }

} // namespace triqs::experimental::lattice
