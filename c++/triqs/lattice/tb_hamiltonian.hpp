c++/triqs/lattice/tb_hamiltonian.hpp#pragma once
#include <nda/nda.hpp>
#include <cassert>
#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include "fourier_polynomial.hpp"
#include "superlattice.hpp"

// FIXME : put in triqs lattice
// FP as well.
namespace triqs {

  class tb_hamiltonian : public fourier_polynomial<2, 3> {

    static constexpr auto r_all = nda::range::all;

    public:
    tb_hamiltonian(std::vector<std::array<long, 3>> Rs, std::vector<nda::array<dcomplex, 2>> hoppings)
       : fourier_polynomial<2, 3>(std::move(Rs), std::move(hoppings)) {};

    C2PY_IGNORE tb_hamiltonian(fourier_polynomial<2, 3> fp) : fourier_polynomial<2, 3>{std::move(fp)} {}

    // ------------------------ Accessors ----------------------------
    ///
    [[nodiscard]] auto const &hoppings() const { return this->get_coefficients(); }

    /** 
     * Provide an iterator of tuples of $$(R, t_{R, ab})$$
     * @return elements : tuple of (R, t_{R,ba}) pairs
     */
    [[nodiscard]] auto elements() const { return itertools::zip(this->get_R_list(), this->get_coefficients()); }

    ///
    [[nodiscard]] long n_orbitals() const { return this->get_coefficients()[0].extent(0); }

    //------------------- band basis energy functions ---------------------------

    /**
      * @brief Function to take a list of kpoints and return a matrix of of energies in orbital space.
      *
      * $$ H_{k,mn} = U_{\mathbf{k},ma}^\dagger (\sum_j t(\mathbf{R}_j)_{ab} * exp(2 \pi i * \mathbf{k} * \mathbf{R}_j) U_{\mathbf{k},nb} $$
      *
      * with lattice displacements {R_j} and associated overlap (hopping) matrices {t(\mathbf{R}_j)_{ab}}
      * k needs to be represented in units of the reciprocal lattice vectors (from 0 to 1)
      *
      * @param k k-points as an array of [nk, 3] in units of the reciprocal lattice vectors
      * @return energies as a matrix of shape [nk, nbands] of band-basis energies as real doubles
      */
<<<<<<< HEAD
=======
    // DOC FIXME : MORE PRECISE
>>>>>>> bdea3b96 (improve bz integration options + docstrings for bz int, gloc)
    nda::array<double, 2> eigenvalues(nda::array_view<double, 2> k) const;

    // REFACTOR -- reformat this and also move it to cpp file
    // simpler : R vectors, dimension, ... not the hopping
    friend std::ostream &operator<<(std::ostream &out, tb_hamiltonian const &tb) {
      out << "tb_hamiltonian [";
      for (auto const &[R, C] : tb.elements()) out << "\n   " << R << " : " << C;
      return out << " ]";
    }

    // ------------------- HDF5 Read / Write -------------------

    // TODO would we like to update what these are called in the HDF5?
    [[nodiscard]] inline static std::string hdf5_format() { return "tb_hamiltonian"; }
  };

  // Superlattice folding user function

  /// @brief Fold the tight-binding Hamiltonian into a superlattice
  inline tb_hamiltonian fold(lattice::superlattice const &sl, tb_hamiltonian const &tb) {
    return fold(sl, static_cast<fourier_polynomial<2, 3> const &>(tb));
  }

} // namespace triqs
