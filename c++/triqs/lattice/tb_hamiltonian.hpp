#pragma once
#include <nda/nda.hpp>
#include <cassert>
#include "fourier_polynomial.hpp"
#include "nda/blas/tools.hpp"
#include "nda/layout/range.hpp"
#include <h5/h5.hpp>
#include <itertools/itertools.hpp>

static constexpr auto r_all = nda::range::all;

// TODO : throw an error if any k value outside 0,1?
// TODO do we also want to allow for a vector of kpts, or an iterator (like a mesh type)?

namespace triqs {

  class tb_hamiltonian : public fourier_polynomial<2, 3> {

    static constexpr int kdim = 3;
    long norbitals;

    public:
    tb_hamiltonian(std::vector<std::array<long, kdim>> Rs, std::vector<nda::array<dcomplex, 2>> hoppings)
       : fourier_polynomial<2, kdim>(std::move(Rs), std::move(hoppings)), norbitals(this->get_coefficients()[0].shape(0)){};

    // ------------------------ Accessors ----------------------------
    [[nodiscard]] auto const &get_hoppings() const { return this->get_coefficients(); }
    [[nodiscard]] long n_orbitals() const { return norbitals; }
    /** Provide an iterator of tuples of $$(R, t_{R, ab})$$
    * @return elements : tuple of (R, t_{R,ba}) pairs
     */
    // FIXME this is temporary and I might be in trouble doing this, but we are changing to nda anyway...
    //[[nodiscard]] auto const &elements() const { return itertools::zip(fourier_polynomial<2, kdim>::Rs, fourier_polynomial<2, kdim>::coefficients); }

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

    [[nodiscard]] static std::string hdf5_format() { return "tight_binding"; }

    // Function that writes the solver_core to hdf5 file
    /*     friend void h5_write(h5::group fg, std::string subgroup_name, tb_hamiltonian const &tb) {
      auto grp = fg.create_group(subgroup_name);
      write_hdf5_format(grp, tb);
      h5_write(grp, "displ_vec", tb.Rs);
      h5_write(grp, "overlap_mat_vec", tb.coefficients);
    }

    // Function to read tight_binding object from hdf5 file
    CPP2PY_IGNORE
    static tight_binding h5_read_construct(h5::group g, std::string subgroup_name) {
      auto grp      = g.open_group(subgroup_name);
      auto R        = h5::h5_read<std::vector<std::array<long, kdim>>>(grp, "displ_vec");
      auto hoppings = h5::h5_read<std::vector<nda::matrix<dcomplex>>>(grp, "overlap_mat_vec");
      return tb_hamiltonian(R, hoppings);
    } */
  };
} // namespace triqs
