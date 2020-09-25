// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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

    // Construct a Green function given a Mesh, a Target and a Memory Layout
    template <typename Mesh, typename T, int R> gf<Mesh, typename T::target_t> make_gf(Mesh m, T const &t, memory_layout_t<R> const &ml) {
      return {std::move(m), t.shape(), ml};
    }

    // Construct a Green function given a Mesh, and a Target
    template <typename Mesh, typename T> gf<Mesh, typename T::target_t> make_gf(Mesh m, T const &t) { return {std::move(m), t.shape()}; }

    // Create a Green function from another gf or view, changing the memory layout
    template <typename G> typename G::regular_type make_gf(G const &g, typename G::data_memory_layout_t const &ml) {
      return {g.mesh(), g.data(), ml, g.indices()};
    }

    // Create a Green function from another gf or view
    template <typename G> typename std::decay_t<G>::regular_type make_gf(G &&g) { return {std::forward<G>(g)}; }

  } // namespace gfs
} // namespace triqs
