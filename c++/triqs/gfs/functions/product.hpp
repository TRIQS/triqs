// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Nils Wentzell

#pragma once

namespace triqs {
  namespace gfs {

    //-------------------------------------------------------
    // closest mesh point on the grid
    // ------------------------------------------------------

    template <typename... Ms, typename Target> struct gf_closest_point<cartesian_product<Ms...>, Target> {
      using index_t = typename gf_mesh<cartesian_product<Ms...>>::index_t;

      template <typename M, typename... T, size_t... Is>
      static index_t _impl(M const &m, closest_pt_wrap<T...> const &p, std::index_sequence<Is...>) {
        return index_t(gf_closest_point<Ms, Target>::invoke(std::get<Is>(m), closest_pt_wrap<T>{std::get<Is>(p.value_tuple)})...);
      }

      template <typename M, typename... T> static index_t invoke(M const &m, closest_pt_wrap<T...> const &p) {
        return _impl(m, p, std::index_sequence_for<T...>{});
      }
    };
  } // namespace gfs
} // namespace triqs
