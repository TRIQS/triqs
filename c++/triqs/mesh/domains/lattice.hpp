#pragma once

#include <fmt/core.h>
#include <nda/h5.hpp>

#include <triqs/utility/exceptions.hpp>
#include "../utils.hpp"

namespace triqs::mesh {

  // FIXME : remove ... .
  // operation at the level of mesh_point !
  struct lattice_point {
    std::array<long, 3> idx          = {0, 0, 0};
    nda::matrix<double> const *units = nullptr;

    using cast_t = nda::stack_vector<double, 3>;
    operator cast_t() const {
      cast_t M{};
      if (units==nullptr) // FIXME : or can we assert it is not !?
        for (int i = 0; i < 3; i++) M[i] = idx[i];
      else 
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) M[i] += idx[j] * (*units)(j, i);
      return M;
    }
    operator std::array<long, 3> const &() const { return idx; }

    friend std::ostream &operator<<(std::ostream &out, lattice_point const &x) { return out << (cast_t)x; }

  };

  inline lattice_point operator+(lattice_point const &x, lattice_point const &y) {
    EXPECTS(x.units == y.units);
    return {x.idx + y.idx, x.units};
  }

  inline lattice_point operator-(lattice_point const &x, lattice_point const &y) {
    EXPECTS(x.units == y.units);
    return {x.idx - y.idx, x.units};
  }

  inline lattice_point operator-(lattice_point const &x) { return {-x.idx, x.units}; }

} // namespace triqs::mesh
