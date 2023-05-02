// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once

#include "../../arrays.hpp"

namespace triqs::gfs {

  using dcomplex = std::complex<double>;

  using nda::array;
  using nda::array_view;
  using nda::ellipsis;
  using triqs::arrays::make_shape;
  using nda::matrix;
  using nda::matrix_const_view;
  using nda::matrix_view;

  using utility::factory;

  // Using from mesh namespace
  using mesh::Mesh;
  using mesh::all_t;
  using mesh::Boson;
  using mesh::closest_mesh_pt;
  //using mesh::closest_pt_wrap;
  using mesh::Fermion;
  using mesh::n_variables;
  using mesh::matsubara_freq;
  using mesh::statistic_enum;

  /*----------------------------------------------------------
   *  Evaluator
   *--------------------------------------------------------*/

  // gf_evaluator regroup functions to evaluate the function.
  template <Mesh M> struct gf_evaluator;

  // the policy
  struct default_evaluator {
    template <typename Mesh> using evaluator_t = gf_evaluator<Mesh>;
  };

  /*----------------------------------------------------------
   *  HDF5
   *  Traits to read/write in hdf5 files.
   *  Can be specialized for some case (Cf block). Defined below
   *--------------------------------------------------------*/

  template <typename Mesh, typename Target> struct gf_h5_rw;

  namespace detail { 

    template<auto Positions, Mesh M>
    auto filter_mesh(M const & m) { 
      static_assert(Positions.size() > 0);
      if constexpr (Positions.size() == 1) {
	return std::get<Positions[0]>(m);
      } else {
        return [&]<size_t... Is>(std::index_sequence<Is...>) {
          return mesh::prod{std::get<Positions[Is]>(m)...}; 
        }(std::make_index_sequence<Positions.size()>{});
      }
    }

    template<size_t L> 
    constexpr std::array<int, L> compute_position(auto const &filter) { 
      std::array<int, L> r{};
      int ii = 0;
      for (int i = 0; i<filter.size() and ii<L; ++i) {
        r[ii] = i;
        ii+= int(filter[i]);
      }
      return r;  
    }

    all_t to_datidx(Mesh auto const &m, all_t const &x) { return {}; }
    auto to_datidx(Mesh auto const &m, auto const &x) { return m.to_datidx(x); }

    template <Mesh M, typename... XS>
    bool eval_to_zero(M const & m, XS const &... xs) {
      if constexpr (sizeof...(XS) > 1) {
        return [&]<std::size_t...Is>(std::index_sequence<Is...>) {
          return (eval_to_zero(std::get<Is>(m), xs) or ... or false);
        }(std::make_index_sequence<sizeof...(XS)>());
      } else if constexpr (requires { m.eval_to_zero(xs...); }) {
        return m.eval_to_zero(xs...);
      }
      return false;
    }

    template<size_t Is, Mesh M> 
    auto const & extract_mesh(M const & m) { 
      static_assert( (Is==0) or mesh::is_product<M>);
      if constexpr (mesh::is_product<M>) return std::get<Is>(m);
      else return m;
    }
  }

} // namespace triqs::gfs
