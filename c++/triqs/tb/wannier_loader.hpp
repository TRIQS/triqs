#pragma once
#include <nda/nda.hpp>
#include "./tb_hamiltonian.hpp"

namespace triqs {
  namespace tb {

    using dcomplex = std::complex<double>;

    /** @brief Reads in Wannier90 _tb.dat file and produces the R vectors in crystal coordinates, 
   * tight binding parameters, and position matrix elements in the real-space Wannier basis 
   * 
   * @param w90_path_and_seedname String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90; 
   *     directory should include path/seedname_tb.dat, where "path/seedname" is the provided string. 
   * 
   * @return tuple containing R[nR, 3], HR[nR, nOrb, nOrb], r_position_op[nR, nOrb, nOrb, 3]. 
   *    HR is in units of eV, R is in crystal coordinates, position_op is in Angstrom.
   */
    std::tuple<std::vector<std::array<long, 3>>, std::vector<nda::array<dcomplex, 2>>, std::vector<nda::array<dcomplex, 3>>>
    read_wannier90_tb_data(std::string const &w90_path_and_seedname);

    /** @brief Reads in Wannier90 _tb.dat file and uses the contents to create a tb_Hamiltonian object. 
   * 
   * @param w90_path_and_seedname String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90; 
   *     directory should include path/seedname_tb.dat, where "path/seedname" is the provided string. 
   * 
   * @return tb_Hamiltonian object, where units of energy are in eV 
   */
    inline tb_hamiltonian make_tb_hamiltonian_from_w90(std::string const &w90_path_and_seedname) {
      auto [Rs, hoppings, _] = read_wannier90_tb_data(w90_path_and_seedname);
      return {std::move(Rs), std::move(hoppings)};
    };

  } // namespace tb
} // namespace triqs