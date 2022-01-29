// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include "triqs/utility/tuple_tools.hpp"
#include <tuple>

namespace triqs::mesh {

  template <typename... Domains> struct domain_product {
    using point_t = std::tuple<typename Domains::point_t...>;
    std::tuple<Domains...> domains;

    domain_product() = default;
    domain_product(std::tuple<Domains...> const &dom_tpl) : domains(dom_tpl) {}
    domain_product(std::tuple<Domains...> &&dom_tpl) : domains(std::move(dom_tpl)) {}
    domain_product(Domains const &...doms) : domains(doms...) {}

    [[nodiscard]] bool contains(point_t const &pt) const {
      return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_within_boundary(arg)); }, domains, pt, true);
    }

    friend bool operator==(domain_product const &D1, domain_product const &D2) { return D1.domains == D2.domains; }
    // implement boost serializable, hdf5 if needed... (done at the mesh level).
  };
} // namespace triqs::mesh
