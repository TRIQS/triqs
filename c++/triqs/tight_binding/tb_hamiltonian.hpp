#pragma once
#include <nda/nda.hpp>
#include <cassert>
#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include "fourier_polynomial.hpp"
#include "superlattice.hpp"
#include <nda/h5.hpp>

namespace triqs {
  namespace tb {

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
     * @brief Provide an iterator of tuples of $$(R, t_{R, ab})$$
     * @return elements : tuple of (R, t_{R,ba}) pairs
     */
      [[nodiscard]] auto elements() const { return itertools::zip(this->get_R_list(), this->get_coefficients()); }

      /** 
     * @brief Provide number of orbitals (the dimension of the stored Hamiltonian)
     * @return n_orbitals 
     */
      [[nodiscard]] long n_orbitals() const { return this->get_coefficients()[0].extent(0); }

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
      * @return tuple containing band-basis energies as a matrix of real doubles with shape [nk, nbands] and complex eigenvectors as a matrix of shape [nk,nBand,nOrbitals]
      */
      std::pair<nda::array<double, 2>, nda::array<dcomplex, 3>> eigenvectors(nda::array_view<double, 2> k) const;

      // ------------------- Comparison -------------------
      bool operator==(tb_hamiltonian const &tb) const {
        return this->get_coefficients() == tb.get_coefficients() && this->get_R_list() == tb.get_R_list();
      }

      bool operator!=(tb_hamiltonian const &tb) const { return !(operator==(tb)); }

      // ------------------- Read / Write -------------------------------

      friend std::ostream &operator<<(std::ostream &out, tb_hamiltonian const &tb) {
        out << "tb_hamiltonian consisting of " << tb.get_R_list().size() << " lattice components (R)";
        out << " with hopping dimension (n_orbitals) " << tb.n_orbitals() << "x" << tb.n_orbitals();
        // if it's a small one, we can print it:
        if (tb.get_R_list().size() < 12) {
          out << "\n[";
          for (auto const &[R, C] : tb.elements()) out << "\n   " << R << " : " << C;
          out << " ]";
        }
        return out;
      }

      [[nodiscard]] inline static std::string hdf5_format() { return "tb_hamiltonian"; }

      /// Function that writes the tb_Hamiltonian to hdf5 file
      friend void h5_write(h5::group fg, std::string subgroup_name, tb_hamiltonian const &tb) {
        auto grp = fg.create_group(subgroup_name);
        write_hdf5_format(grp, tb);
        h5_write(grp, "lattice_vectors_R", tb.get_R_list());
        h5_write(grp, "hoppings", tb.hoppings());
      }

      // Function to read tight_binding object from hdf5 file
      CPP2PY_IGNORE
      static tb_hamiltonian h5_read_construct(h5::group g, std::string subgroup_name) {
        auto grp      = g.open_group(subgroup_name);
        auto R        = h5::h5_read<std::vector<std::array<long, 3>>>(grp, "lattice_vectors_R");
        auto hoppings = h5::h5_read<std::vector<nda::array<dcomplex, 2>>>(grp, "hoppings");
        return tb_hamiltonian(R, hoppings);
      }
    };

    // Superlattice folding user function

    /// @brief Fold the tight-binding Hamiltonian into a superlattice
    inline tb_hamiltonian fold(superlattice const &sl, tb_hamiltonian const &tb) {
      return fold(sl, static_cast<fourier_polynomial<2, 3> const &>(tb));
    }

  } // namespace tb
} // namespace triqs
