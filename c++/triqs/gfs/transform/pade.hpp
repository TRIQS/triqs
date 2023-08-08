// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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

namespace triqs::gfs {

  void pade(gf_view<refreq, scalar_valued> gr, gf_const_view<imfreq, scalar_valued> gw, int n_points, double freq_offset);

  template <MemoryGf<refreq> GR, MemoryGf<imfreq> GW>
  void pade(GR &gr, GW const &gw, int n_points, double freq_offset)
    requires(GR::target_rank > 0 && GW::target_rank > 0)
  {
    EXPECTS(gr.target_shape() == gw.target_shape());
    for (auto argtpl : gr.target_indices()) {
      std::apply([&](auto &&...args) { pade(slice_target_to_scalar(gr, args...), slice_target_to_scalar(gw, args...), n_points, freq_offset); },
                 argtpl);
    }
  }
} // namespace triqs::gfs
