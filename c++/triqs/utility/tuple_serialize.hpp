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
// Authors: Olivier Parcollet, Nils Wentzell

#ifndef TRIQS_UTILITY_TUPLE_SERIALIZE_H
#define TRIQS_UTILITY_TUPLE_SERIALIZE_H

#include <tuple>

namespace boost {
  namespace serialization {

    template <int pos> struct tuple_serialize_impl {
      template <typename Archive, typename T> void operator()(Archive &ar, T &t) {
        ar &std::get<std::tuple_size<T>::value - 1 - pos>(t);
        tuple_serialize_impl<pos - 1>()(ar, t);
      }
    };

    template <> struct tuple_serialize_impl<0> {
      template <typename Archive, typename T> void operator()(Archive &ar, T &t) { ar &std::get<std::tuple_size<T>::value - 1>(t); }
    };

    template <typename Archive, typename... ElementTypes> void serialize(Archive &ar, std::tuple<ElementTypes...> &t, const unsigned int version) {
      tuple_serialize_impl<sizeof...(ElementTypes) - 1>()(ar, t);
    }

  } // namespace serialization
} // namespace boost

#endif
