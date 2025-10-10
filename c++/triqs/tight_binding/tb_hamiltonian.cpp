#include "tb_hamiltonian.hpp"

namespace triqs {
  namespace tb {

    nda::array<double, 2> tb_hamiltonian::eigenvalues(nda::array_view<double, 2> k) const {
      // energies in the shape of nk, nbands
      long nk         = k.shape()[0];
      auto energies   = nda::matrix<double>(nk, n_orbitals());
      auto Ek_orbital = nda::array<dcomplex, 3>{this->operator()(k)};
      for (auto ik : nda::range(nk)) { energies(ik, r_all) = nda::linalg::eigenvalues(Ek_orbital(ik, nda::ellipsis())); }
      return energies;
    }
  } // namespace tb
}; // namespace triqs
