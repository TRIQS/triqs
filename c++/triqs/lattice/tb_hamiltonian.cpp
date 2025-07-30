#include "tb_hamiltonian.hpp"

namespace triqs {

  nda::array<double, 2> triqs::tb_hamiltonian::eigenvalues(nda::array_view<double, 2> k) const {
    // energies in the shape of nk, nbands
    long nk         = k.shape()[0];
    auto energies   = nda::matrix<double>(nk, n_orbitals());
    auto Ek_orbital = nda::array<dcomplex, 3>{this->operator()(k)};
    for (auto ik : nda::range(nk)) { energies(ik, r_all) = nda::linalg::eigenvalues(Ek_orbital(ik, nda::ellipsis())); }
    return energies;
  }

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
}; // namespace triqs