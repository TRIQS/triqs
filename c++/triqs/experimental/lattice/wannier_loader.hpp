#pragma once
#include <nda/nda.hpp>
#include "./tb_hk.hpp"

namespace triqs::experimental::lattice {

  using dcomplex = std::complex<double>;

  /** @brief Reads in Wannier90 *_tb.dat file and produces the R vectors in crystal coordinates,
   * tight binding parameters, position matrix elements in the real-space Wannier basis, and lattice vectors
   *
   * @param w90_path_and_seedname String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
   *     directory should include path/seedname_tb.dat, where "path/seedname" is the provided string.
   *
   * @return tuple containing HR[nR, nOrb, nOrb], R[nR, 3], r_position_op[nR, nOrb, nOrb, 3], lattice_vectors[3,3]
   *    HR is in units of eV, R is in crystal/lattice coordinates, position_op is in Angstrom, lattice_vectors in angstrom
   */
  std::tuple<std::vector<std::array<long, 3>>, std::vector<nda::array<dcomplex, 2>>, std::vector<nda::array<dcomplex, 3>>, nda::matrix<double>>
  read_wannier90_tb_data(std::string const &w90_path_and_seedname);

  /** @brief Reads in Wannier90 *_tb.dat file and uses the contents to create a tb_hk object.
   *
   * @param w90_path_and_seedname String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
   *     directory should include path/seedname_tb.dat, where "path/seedname" is the provided string.
   *

   * @return tb_hk object, where units of energy are in eV.
   */
  inline tb_hk make_tb_hk_from_w90_tb_file(std::string const &w90_path_and_seedname) {
    auto [Rs, hoppings, _, _] = read_wannier90_tb_data(w90_path_and_seedname);
    return {std::move(Rs), std::move(hoppings)};
  };

  /** @brief Reads in Wannier90 *_hr.dat file and produces the R vectors in crystal coordinates and tight binding parameters
   *
   * @param w90_path_and_seedname String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
   *     directory should include path/seedname_hr.dat, where "path/seedname" is the provided string.
   *
   * @return tuple containing R[nR, 3], HR[nR, nOrb, nOrb]
   *    R is in crystal/lattice coordinates, HR is in units of eV
   */
  std::tuple<std::vector<std::array<long, 3>>, std::vector<nda::array<dcomplex, 2>>> read_wannier90_hr_data(std::string const &w90_path_and_seedname);

  /** @brief Reads in Wannier90 *_hr.dat file and uses the contents to create a tb_hk object.
   *
   * @param w90_path_and_seedname String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
   *     directory should include path/seedname_tb.dat, where "path/seedname" is the provided string.
   *
   * @return tb_hk object, where units of energy are in eV.
   */
  inline tb_hk make_tb_hk_from_w90_hr_file(std::string const &w90_path_and_seedname) {
    auto [Rs, hoppings] = read_wannier90_hr_data(w90_path_and_seedname);
    return {std::move(Rs), std::move(hoppings)};
  };

} // namespace triqs::experimental::lattice