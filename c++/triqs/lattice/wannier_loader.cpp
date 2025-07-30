#include "wannier_loader.hpp"
#include <fstream> // std::ifstream
#include <stdexcept>
#include "../utility/generator.hpp"

namespace triqs {
  template <typename T> T read(std::ifstream &file) {
    T x;
    file >> x;
    return x;
  }

  template <typename T> generator<T> read_N(std::ifstream &file, int n) {
    for ([[maybe_unused]] auto i : nda::range(n)) co_yield read<T>(file);
  }

  // ------------------------------------------------------

  // TODO do we always want to return the position operator?
  std::tuple<std::vector<std::array<long, 3>>, std::vector<nda::array<dcomplex, 2>>, std::vector<nda::array<dcomplex, 3>>>
  read_wannier90_tb_data(std::string const &w90_path_and_seedname) {

    // open the file
    std::string tb_filename = w90_path_and_seedname + "_tb.dat";
    std::ifstream file(tb_filename);
    if (!file.is_open()) throw std::runtime_error("Could not open *_tb.dat file with provided name " + tb_filename);

    // skip first line, it's a 5 item line of the date / time
    for ([[maybe_unused]] auto i : read_N<std::string>(file, 5)) {}

    // read the lattice vectors -- unused for now. TODO Do we need these?
    for ([[maybe_unused]] auto x : {0, 1, 2})
      for ([[maybe_unused]] auto y : read_N<double>(file, 3)) {}

    // next two lines have number of wannier centers, number of R vectors
    int n_wannier = read<int>(file);
    int n_r       = read<int>(file);

    // read in the R vector degeneracies
    std::vector<int> r_degeneracies;
    for (auto i : read_N<int>(file, n_r)) { r_degeneracies.push_back(i); };

    // set up containers for R and H_r
    std::vector<std::array<long, 3>> r_vectors;
    std::vector<nda::array<dcomplex, 2>> H_r;
    std::vector<nda::array<dcomplex, 3>> position_op_r;

    for ([[maybe_unused]] auto r : nda::range(n_r)) {
      r_vectors.emplace_back(std::array<long, 3>{0, 0, 0});
      H_r.emplace_back(n_wannier, n_wannier);
      position_op_r.emplace_back(n_wannier, n_wannier, 3);
    }

    // read the R vectors and the Wannier Hamiltonian -------
    std::array<long, 3> R;
    for (auto ir : nda::range(n_r)) {

      file >> R[0] >> R[1] >> R[2];
      for ([[maybe_unused]] auto [x, y] : product(nda::range(n_wannier), nda::range(n_wannier))) {

        double re, im;
        int ii, jj; // read these from the file rather than as loop indices
        file >> ii >> jj >> re >> im;

        // check state
        // NOTE if we want to use ws_cells vec data, we will likely want to use all wannier indices instead
        // (wannier90 indexes this from 1)
        if ((ii == 1) and (jj == 1))
          for (int i : {0, 1, 2}) r_vectors[ir][i] = R[i];
        //else if (R != r_vectors(ir)) // TODO is there a neat way to do this?
        //  throw "Rvector does not match that of Wannier centers in the same cell.";

        // merge degeneracy vector into Wannier Hamiltonian (note, W90 indexes from 1)
        // TODO these are in eV, do we care about units? ALSO any reason to do this degen merging elsewhere?
        H_r[ir](ii - 1, jj - 1) = dcomplex{re, im} / r_degeneracies[ir];
      }
    }

    // read in the position operator elements in the Wannier basis --------------------
    // These appear at the end of the tb.dat file in units of Angstrom
    // TODO These are in units of Angstrom -- do we want to convert them?
    for (auto ir : nda::range(n_r)) {
      // R vector is listed before each r_matrix block in the file
      file >> R[0] >> R[1] >> R[2];
      for ([[maybe_unused]] auto [x, y] : product(nda::range(n_wannier), nda::range(n_wannier))) {

        std::array<double, 3> re, im;
        int ii, jj; // read these from the file rather than as loop indices
        file >> ii >> jj >> re[0] >> im[0] >> re[1] >> im[1] >> re[2] >> im[2];
        // double check that merging in degeneracy info here is ok
        for (auto idx_cart : {0, 1, 2})
          position_op_r[ir](ii - 1, jj - 1, idx_cart) = dcomplex{re[idx_cart], im[idx_cart]} / r_degeneracies[ir]; // in Angstrom
      }
    }
    return {std::move(r_vectors), std::move(H_r), std::move(position_op_r)};
  }
} // namespace triqs
