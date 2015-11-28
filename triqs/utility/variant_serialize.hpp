/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by I. Krivenko
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include "variant.hpp"

#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {

template<typename Archive> struct variant_serialize_saver {
  Archive & ar;
  variant_serialize_saver(Archive & ar) : ar(ar) {}
  template<typename T> void operator()(T x) { ar << x; }
};

template<typename Archive> struct variant_serialize_loader {
  Archive & ar;
  bool initialize;
  variant_serialize_loader(Archive & ar, bool initialize) : ar(ar), initialize(initialize) {}
  template<typename T> void operator()(T & x) {
    if(initialize) ::new(&x) T();
    ar >> x;
  }
};

template<typename Archive, typename... Types>
inline void serialize(Archive & ar, triqs::utility::variant<Types...> & v, const unsigned int version) {
  split_free(ar, v, version);
}

template<typename Archive, typename... Types>
void save(Archive & ar, triqs::utility::variant<Types...> const& v, const unsigned int version) {
  ar << v.type_id;
  apply_visitor(variant_serialize_saver<Archive>(ar),v);
}

template<typename Archive, typename... Types>
void load(Archive & ar, triqs::utility::variant<Types...> & v, const unsigned int version) {
  int new_type_id; ar >> new_type_id;
  if(v.type_id != new_type_id) {
    v.destroy();
    v.type_id = new_type_id;
    apply_visitor(variant_serialize_loader<Archive>(ar,true),v);
  } else
    apply_visitor(variant_serialize_loader<Archive>(ar,false),v);
}

} // namespace serialization
} // namespace boost
