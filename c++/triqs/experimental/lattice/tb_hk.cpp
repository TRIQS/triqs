#include "./tb_hk.hpp"
#include "../../utility/exceptions.hpp"

#include <nda/nda.hpp>

#include <utility>

namespace triqs::experimental::lattice {

  void tb_hk::check_hoppings() {

    // checking displacements and hopping matrices
    if (n_R() != hoppings().size()) TRIQS_RUNTIME_ERROR << " Number of Rs != Number of hoppings";
    for (int i = 0; i < n_R(); ++i) {
      if (first_dim(hoppings()[i]) != n_orbitals())
        TRIQS_RUNTIME_ERROR << "hopping dimension 1 inconsistent with n_orbitals, for R number " << i << " " << first_dim(hoppings()[i])
                            << " instead of " << n_orbitals();
      if (second_dim(hoppings()[i]) != n_orbitals())
        TRIQS_RUNTIME_ERROR << "the second dim matrix is of size " << second_dim(hoppings()[i]) << " instead of " << n_orbitals();

      // check hermiticity of hoppings: Hij(+R)= Hji(-R)* by looping of all displacements again
      bool found = false;
      for (int j = 0; j < n_R(); ++j) {
        if (get_R_list()[i] == -get_R_list()[j]) {
          found = true;
          if (max_element(abs(hoppings()[i] - dagger(hoppings()[j]))) > 1.e-12)
            TRIQS_RUNTIME_ERROR << "For displacement " << get_R_list()[i] << " hopping matrix " << hoppings()[i]
                                << "\nis not the hermitian conjugate of matrix for displacement " << get_R_list()[j] << hoppings()[j] << "\n";
          break;
        }
      }
      if (not found) TRIQS_RUNTIME_ERROR << "opposite hopping vector of " << get_R_list()[i] << " cannot be found";
    }
  }

  nda::array<double, 2> tb_hk::eigenvalues(nda::array_view<double, 2> k) const {
    // energies in the shape of nk, nbands
    long nk       = k.shape()[0];
    auto energies = nda::matrix<double>(nk, n_orbitals());
    // use blas call for FT on a block of kpoints
    auto Ek_orbital = nda::array<dcomplex, 3>{this->operator()(k)};
    for (auto ik : nda::range(nk)) { energies(ik, r_all) = nda::linalg::eigvalsh(Ek_orbital(ik, nda::ellipsis())); }
    return energies;
  }

  std::pair<nda::array<double, 2>, nda::array<dcomplex, 3>> tb_hk::eigenvectors(nda::array_view<double, 2> k) const {
    // energies in the shape of nk, nbands
    long nk           = k.shape()[0];
    auto energies     = nda::matrix<double>(nk, n_orbitals());
    auto eigenvectors = nda::array<dcomplex, 3>(nk, n_orbitals(), n_orbitals());
    // use blas call for FT on a block of kpoints

    auto Ek_orbital = nda::array<dcomplex, 3>{this->operator()(k)};
    for (auto ik : nda::range(nk)) {
      auto [en, eigvec]              = nda::linalg::eigh(Ek_orbital(ik, nda::ellipsis()));
      energies(ik, r_all)            = en;
      eigenvectors(ik, r_all, r_all) = eigvec;
    }
    return {energies, eigenvectors};
  }

}; // namespace triqs::experimental::lattice
