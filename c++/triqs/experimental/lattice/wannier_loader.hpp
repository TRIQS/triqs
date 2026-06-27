#pragma once

#include "./tb_hk.hpp"

#include <nda/nda.hpp>

#include <array>
#include <complex>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace triqs::experimental::lattice {

  using dcomplex = std::complex<double>;

  /**
   * @addtogroup triqs-experimental-lattice
   * @{
   */

  /// A list of real-space lattice vectors \f$ \mathbf{R} \f$, in crystal (lattice) coordinates.
  using r_vector_list_t = std::vector<std::array<long, 3>>;

  /// A list of hopping (overlap) matrices \f$ t(\mathbf{R}) \f$, one per lattice vector.
  using hopping_list_t = std::vector<nda::array<dcomplex, 2>>;

  /// A list of position-operator matrices in the real-space Wannier basis, one per lattice vector.
  using position_op_list_t = std::vector<nda::array<dcomplex, 3>>;

  /// Data read from a Wannier90 `*_tb.dat` file: lattice vectors, hopping matrices, position operators and UC vectors.
  using w90_tb_data_t = std::tuple<r_vector_list_t, hopping_list_t, position_op_list_t, nda::matrix<double>>;

  /// Data read from a Wannier90 `*_hr.dat` file: lattice vectors and hopping matrices.
  using w90_hr_data_t = std::tuple<r_vector_list_t, hopping_list_t>;

  /**
   * @brief Read a Wannier90 `*_tb.dat` file and return the lattice vectors, hopping matrices, position operators and
   * unit-cell vectors.
   *
   * @details The file is located by appending `_tb.dat` to the given path and seedname. All quantities are returned in
   * the units used by Wannier90: hoppings in eV, lattice vectors \f$ \mathbf{R} \f$ in crystal (lattice) coordinates,
   * position operators in Angstrom and unit-cell vectors in Angstrom.
   *
   * @param w90_path_and_seedname Path to the Wannier90 files including the seedname, such that the file to read is
   * `<w90_path_and_seedname>_tb.dat`.
   * @return Tuple containing the lattice vectors \f$ \mathbf{R} \f$ (shape `[nR, 3]`), the hopping matrices (shape
   * `[nR, nOrb, nOrb]`), the position operators in the real-space Wannier basis (shape `[nR, nOrb, nOrb, 3]`) and the
   * unit-cell vectors (shape `[3, 3]`).
   */
  w90_tb_data_t read_wannier90_tb_data(std::string const &w90_path_and_seedname);

  /**
   * @brief Build a tight-binding Hamiltonian from a Wannier90 `*_tb.dat` file.
   *
   * @details This is a convenience wrapper that reads the file and keeps only the lattice vectors and hopping matrices,
   * discarding the position operators and unit-cell vectors.
   *
   * @param w90_path_and_seedname Path to the Wannier90 files including the seedname, such that the file to read is
   * `<w90_path_and_seedname>_tb.dat`.
   * @return Tight-binding Hamiltonian with hoppings in units of eV.
   */
  inline tb_hk make_tb_hk_from_w90_tb_file(std::string const &w90_path_and_seedname) {
    auto [Rs, hoppings, _unused1, _unused2] = read_wannier90_tb_data(w90_path_and_seedname);
    return {std::move(Rs), std::move(hoppings)};
  };

  /**
   * @brief Read a Wannier90 `*_hr.dat` file and return the lattice vectors and hopping matrices.
   *
   * @details The file is located by appending `_hr.dat` to the given path and seedname. The lattice vectors
   * \f$ \mathbf{R} \f$ are returned in crystal (lattice) coordinates and the hoppings in eV.
   *
   * @param w90_path_and_seedname Path to the Wannier90 files including the seedname, such that the file to read is
   * `<w90_path_and_seedname>_hr.dat`.
   * @return Tuple containing the lattice vectors \f$ \mathbf{R} \f$ (shape `[nR, 3]`) and the hopping matrices (shape
   * `[nR, nOrb, nOrb]`).
   */
  w90_hr_data_t read_wannier90_hr_data(std::string const &w90_path_and_seedname);

  /**
   * @brief Build a tight-binding Hamiltonian from a Wannier90 `*_hr.dat` file.
   *
   * @details This is a convenience wrapper that reads the file and keeps the lattice vectors and hopping matrices.
   *
   * @param w90_path_and_seedname Path to the Wannier90 files including the seedname, such that the file to read is
   * `<w90_path_and_seedname>_hr.dat`.
   * @return Tight-binding Hamiltonian with hoppings in units of eV.
   */
  inline tb_hk make_tb_hk_from_w90_hr_file(std::string const &w90_path_and_seedname) {
    auto [Rs, hoppings] = read_wannier90_hr_data(w90_path_and_seedname);
    return {std::move(Rs), std::move(hoppings)};
  };

  /** @} */

} // namespace triqs::experimental::lattice
