// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2015 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

#pragma once
#include <variant>
#include <boost/serialization/split_free.hpp>

namespace boost {
  namespace serialization {

    template <typename Archive> struct variant_serialize_saver {
      Archive &ar;
      variant_serialize_saver(Archive &ar) : ar(ar) {}
      template <typename T> void operator()(T x) { ar << x; }
    };

    template <typename Archive> struct variant_serialize_loader {
      Archive &ar;
      bool initialize;
      variant_serialize_loader(Archive &ar, bool initialize) : ar(ar), initialize(initialize) {}
      template <typename T> void operator()(T &x) {
        if (initialize) ::new (&x) T();
        ar >> x;
      }
    };

    template <typename Archive, typename... Types>
    inline void serialize(Archive &ar, triqs::utility::variant<Types...> &v, const unsigned int version) {
      split_free(ar, v, version);
    }

    template <typename Archive, typename... Types> void save(Archive &ar, std::variant<Types...> const &v, const unsigned int version) {
      ar << v.type_id;
      visit(variant_serialize_saver<Archive>(ar), v);
    }

    template <typename Archive, typename... Types> void load(Archive &ar, std::variant<Types...> &v, const unsigned int version) {
      int new_type_id;
      ar >> new_type_id;
      if (v.type_id != new_type_id) {
        v.destroy();
        v.type_id = new_type_id;
        visit(variant_serialize_loader<Archive>(ar, true), v);
      } else
        visit(variant_serialize_loader<Archive>(ar, false), v);
    }

  } // namespace serialization
} // namespace boost
