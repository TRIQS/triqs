#include "tb_hamiltonian.hpp"

namespace triqs {
  namespace tb {

    nda::array<double, 2> tb_hamiltonian::eigenvalues(nda::array_view<double, 2> k) const {
      // energies in the shape of nk, nbands
      long nk       = k.shape()[0];
      auto energies = nda::matrix<double>(nk, n_orbitals());
      // use blas call for FT on a block of kpoints
      auto Ek_orbital = nda::array<dcomplex, 3>{this->operator()(k)};
      for (auto ik : nda::range(nk)) { energies(ik, r_all) = nda::linalg::eigenvalues(Ek_orbital(ik, nda::ellipsis())); }
      return energies;
    }

    std::pair<nda::array<double, 2>, nda::array<dcomplex, 3>> tb_hamiltonian::eigenvectors(nda::array_view<double, 2> k) const {
      // energies in the shape of nk, nbands
      long nk           = k.shape()[0];
      auto energies     = nda::matrix<double>(nk, n_orbitals());
      auto eigenvectors = nda::array<dcomplex, 3>(nk, n_orbitals(), n_orbitals());
      // use blas call for FT on a block of kpoints
      auto Ek_orbital = nda::array<dcomplex, 3>{this->operator()(k)};
      for (auto ik : nda::range(nk)) {
        auto [en, eigvec]              = nda::linalg::eigenelements(Ek_orbital(ik, nda::ellipsis()));
        energies(ik, r_all)            = en;
        eigenvectors(ik, r_all, r_all) = eigvec;
      }
      return {energies, eigenvectors};
    }

  } // namespace tb
}; // namespace triqs
