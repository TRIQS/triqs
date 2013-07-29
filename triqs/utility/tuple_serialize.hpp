/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by I. Krivenko, M. Ferrero, O. Parcollet
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

#ifndef TRIQS_UTILITY_TUPLE_SERIALIZE_H
#define TRIQS_UTILITY_TUPLE_SERIALIZE_H

#include <tuple>

namespace boost {
namespace serialization {

template<int pos>
struct tuple_serialize_impl {
    template<typename Archive, typename T>
    void operator()(Archive & ar, T & t) {
        ar & std::get<std::tuple_size<T>::value-1-pos>(t);
        tuple_serialize_impl<pos-1>()(ar, t);
    }
};

template<>
struct tuple_serialize_impl<0> {
   template<typename Archive, typename T>
   void operator() (Archive & ar, T & t) { ar & std::get<std::tuple_size<T>::value-1>(t); }
};
    
template<typename Archive, typename... ElementTypes>
void serialize(Archive & ar, std::tuple<ElementTypes...> & t, const unsigned int version)
{
    tuple_serialize_impl<sizeof...(ElementTypes)-1>()(ar, t);
}

} // namespace serialization
} // namespace boost

#endif